#ifndef HPRINT_HPP
#define HPRINT_HPP

#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <type_traits>

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

/*
 * hprint - A simple, header-only, type-safe C++ printing library.
 *
 * - Uses '%' for specifiers, '`%' for escaping.
 * - In Debug builds (NDEBUG not defined), asserts that argument count matches specifiers.
 * - Extensible for user-defined types via `to_string(const T&)`.
 * - Avoids iostreams.
 */
namespace hprint {
namespace detail {

    // Helper to count specifiers at runtime. This is cheap.
    inline size_t count_specifiers(std::string_view fmt) {
        size_t count = 0;
        for (size_t i = 0; i < fmt.size(); ++i) {
            if (fmt[i] == '%') {
                if (i == 0 || fmt[i - 1] != '`') {
                    count++;
                }
            }
        }
        return count;
    }

    template<typename T, typename = void>
    struct has_adl_to_string : std::false_type {};
    template<typename T>
    struct has_adl_to_string<T, std::void_t<decltype(to_string(std::declval<T>()))>> : std::true_type {};

    template<typename T>
    std::string to_string_converter(const T& val) {
        using DecayedT = std::decay_t<T>;
        if constexpr (std::is_same_v<DecayedT, std::string> || std::is_same_v<DecayedT, std::string_view>) {
            return std::string(val);
        } else if constexpr (std::is_same_v<DecayedT, const char*>) {
            return (val == nullptr) ? std::string{"(null)"} : std::string{val};
        } else if constexpr (std::is_same_v<DecayedT, char>) {
            return {val};
        } else if constexpr (std::is_arithmetic_v<DecayedT> && !std::is_same_v<DecayedT, bool> && !std::is_same_v<DecayedT, char>) {
            return std::to_string(val);
        } else if constexpr (std::is_same_v<DecayedT, bool>) {
            return val ? "true" : "false";
        } else if constexpr (std::is_pointer_v<DecayedT>) {
            char buffer[20];
            std::snprintf(buffer, sizeof(buffer), "%p", static_cast<const void*>(val));
            return buffer;
        } else if constexpr (has_adl_to_string<const T&>::value) {
            return to_string(val);
        } else {
            static_assert(has_adl_to_string<const T&>::value,
                          "hprint: Argument type cannot be converted to a string. "
                          "Please provide a `to_string(const YourType&)` free function "
                          "in your type's namespace.");
            return "";
        }
    }


    template<typename OutputIt, typename... Args>
    void print_impl(OutputIt out_func, std::string_view fmt, Args&&... args) {
        auto arg_tuple = std::make_tuple(to_string_converter(std::forward<Args>(args))...);
        size_t arg_idx = 0;
        size_t last_pos = 0;

        for (size_t i = 0; i < fmt.size(); ++i) {
            if (fmt[i] == '%') {
                if (i > 0 && fmt[i - 1] == '`') {
                    out_func({fmt.data() + last_pos, i - 1 - last_pos});
                    out_func({"%", 1});
                    last_pos = i + 1;
                } else {
                    out_func({fmt.data() + last_pos, i - last_pos});

                    if (arg_idx < sizeof...(Args)) {
                        std::apply(
                            [arg_idx, &out_func](const auto&... strings) {
                                size_t current_idx = 0;
                                ((current_idx++ == arg_idx ? out_func(strings) : (void)0), ...);
                            },
                            arg_tuple);
                    }
                    // If arg_idx >= sizeof...(Args), we are in a release build with
                    // a mismatch. We simply do nothing ("skip over").

                    arg_idx++;
                    last_pos = i + 1;
                }
            }
        }

        if (last_pos < fmt.size()) {
            out_func({fmt.data() + last_pos, fmt.size() - last_pos});
        }
    }

} // namespace detail

/* Prints formatted text to stream. Asserts on argument mismatch in debug builds. */
template<typename... Args>
void fprint(FILE *stream, const char* format_str, Args&&... args) {
    assert(detail::count_specifiers(format_str) == sizeof...(args) &&
           "hprint: Mismatch between number of format specifiers ('%') and arguments.");

    auto stdout_writer = [&stream](std::string_view sv) {
        fwrite(sv.data(), 1, sv.size(), stream);
    };
    detail::print_impl(stdout_writer, format_str, std::forward<Args>(args)...);
}

/* Prints formatted text to stream, followed by a newline. */
template<typename... Args>
void fprintln(FILE *stream, const char* format_str, Args&&... args) {
    assert(detail::count_specifiers(format_str) == sizeof...(args) &&
           "hprint: Mismatch between number of format specifiers ('%') and arguments.");

    fprint(stream, format_str, std::forward<Args>(args)...);
    fputc('\n', stdout);
}


/* Prints formatted text to stdout. Asserts on argument mismatch in debug builds. */
template<typename... Args>
void print(const char* format_str, Args&&... args) {
    fprint(stdout, format_str, std::forward<Args>(args)...);
}

/* Prints formatted text to stdout, followed by a newline. */
template<typename... Args>
void println(const char* format_str, Args&&... args) {
    fprintln(stdout, format_str, std::forward<Args>(args)...);
}

/* Formats text into a std::string. */
template<typename... Args>
[[nodiscard]] std::string sprint(const char* format_str, Args&&... args) {
    assert(detail::count_specifiers(format_str) == sizeof...(args) &&
           "hprint: Mismatch between number of format specifiers ('%') and arguments.");

    std::string result;
    result.reserve(std::string_view(format_str).length() + (sizeof...(Args) * 8));

    auto string_appender = [&result](std::string_view sv) {
        result.append(sv);
    };

    detail::print_impl(string_appender, format_str, std::forward<Args>(args)...);
    return result;
}

} // namespace hprint

#endif // HPRINT_HPP
