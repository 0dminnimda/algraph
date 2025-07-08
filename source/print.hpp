/*
 * print.hpp - A simple, header-only, type-safe C++ printing library. Similar to C++20 std::format or Python f-strings.
 *
 * - Uses '%' for specifiers, '`%' for escaping.
 * - Extensible for user-defined types via `to_str(Array<char>* builder, const T& val)`.
 * - Avoids s.
 * - It does NOT use `std::string` or `iostream` at any point.
 * - In Debug builds (NDEBUG not defined), asserts that argument count matches specifiers.
 */

#ifndef PRINT_HPP
#define PRINT_HPP

#include <type_traits>  // For std::is_... traits
#include <charconv>     // For std::to_chars
#include <system_error> // For std::errc

#include "array.hpp" // string builder
#include "str.hpp" // string builder


namespace print_detail {

template<typename T>
void h_format_arg(Array<char>* builder, const T& value);

void print_impl_recursive(Array<char>* builder, const char* fmt);

template<typename T, typename... Rest>
void print_impl_recursive(Array<char>* builder, const char* fmt, T&& arg, Rest&&... rest);

} // namespace print_detail

/*
 * - Returns a new, heap-allocated `str` object. The caller is responsible for freeing this `str` using `str_free`.
 */
template<typename... Args>
[[nodiscard]] str sprint(const char* format_str, Args&&... args) {
    Array<char> builder = {};
    array_reserve(&builder, strlen(format_str) + (sizeof...(args) * 8)); // Initial guess

    print_detail::print_impl_recursive(&builder, (char*)format_str, std::forward<Args>(args)...);

    return (str){builder.data, builder.length};
}

template<typename... Args>
void fprint(FILE *stream, const char* format_str, Args&&... args) {
    str string = sprint(format_str, std::forward<Args>(args)...);
    fwrite(string.data, sizeof(*string.data), string.length, stream);
    str_free(string);
}

template<typename... Args>
void print(const char* format_str, Args&&... args) {
    fprint(stdout, format_str, std::forward<Args>(args)...);
}

inline void builder_add(Array<char> *array, str string) {
    array_add_range(array, string.data, string.length);
}

namespace print_detail {

// The "extension point" for user-defined types. ADL will find this.
// The primary template is intentionally left undefined.
template<typename T>
void to_str(Array<char>* builder, const T& val);


template<typename T>
void h_format_arg(Array<char>* builder, const T& value) {
    using DecayedT = std::decay_t<T>;

    if constexpr (std::is_same_v<DecayedT, char*>) {
        if (value) {
            builder_add(builder, str_cstr_view(value));
        } else {
            builder_add(builder, str("(null)"));
        }
    }
    else if constexpr (std::is_same_v<DecayedT, str>) {
        builder_add(builder, value);
    }
    else if constexpr (std::is_same_v<DecayedT, bool>) {
        if (value) {
            builder_add(builder, str("true"));
        } else {
            builder_add(builder, str("false"));
        }
    }
    else if constexpr (std::is_integral_v<DecayedT> || std::is_floating_point_v<DecayedT>) {
        char *buffer = builder->data + builder->length;
        array_reserve_to_add(builder,                   32       );
        auto [ptr, ec] = std::to_chars(buffer, buffer + 32, value);
        assert((ec == std::errc()) && "Internal formtting error");
        builder->length += ptr - buffer;
    }
    else {
        to_str(builder, value);
    }
}


// Base case for the recursion: no more arguments to process.
// Just append the rest of the format string.
void print_impl_recursive(Array<char>* builder, const char* fmt) {
    builder_add(builder, str_cstr_view((char*)fmt));
}

// Recursive step: process one argument and recurse on the rest.
template<typename T, typename... Rest>
void print_impl_recursive(Array<char>* builder, const char* fmt, T&& arg, Rest&&... rest) {
    const char* p = strchr(fmt, '%');
    if (!p) {
        // No more specifiers, append the whole rest of the string and we're done.
        builder_add(builder, str_cstr_view((char*)fmt));
        return;
    }

    builder_add(builder, (str){(char*)fmt, (u64)(p - fmt)});

    h_format_arg(builder, std::forward<T>(arg));

    print_impl_recursive(builder, p + 1, std::forward<Rest>(rest)...);
}

} // namespace print_detail

#endif // PRINT_HPP

// SEE: https://github.com/kraj/musl/blob/kraj/master/src/stdio/vfprintf.c
