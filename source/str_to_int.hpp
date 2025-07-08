/* Treats as much characters as digits as it can.
 * Makes a slice to the string after the parsed integer. */

#include <type_traits>
#include <limits>
#include <cstdint>

enum S2I_Result {
    S2I_OK,
    S2I_NOT_FOUND,
    S2I_OUT_OF_RANGE
};

/* TODO: ignore leading zeros,
 * TODO: except leading 0b, 0o and 0x.

 * TODO: Allow to pass in a target base.

 * TODO: Allow single underscores in the number
 * (except the first, last and characteres right after 0b,0o,0x).
 * Not allowed: _234, 456_, 0x_F, 0xF_
 * Allowed: 2_3_4, 0xF_A
 */

/* SEE:
 * https://github.com/lattera/glibc/blob/895ef79e04a953cac1493863bcae29ad85657ee1/stdlib/strtol_l.c
 * https://github.com/python/cpython/blob/6557af669899f18f8d123f8e1b6c3380d502c519/Objects/longobject.c#L2518
 */

template <typename T>
S2I_Result str_to_int_and_consume(str *string, T *value);

template <typename T>
S2I_Result str_to_int(str string, T *value) {
    str temporary = string;
    return str_to_int_and_consume(&temporary, value);
}

template <typename T>
S2I_Result str_to_int_and_consume(str *string, T *value) {
    static_assert(std::is_integral<T>::value, "T must be an integer type");

    str temp_str = *string;

    if (temp_str.length == 0) {
        return S2I_NOT_FOUND;
    }

    bool is_negative = false;
    size_t consumed_prefix = 0;

    if (std::is_signed<T>::value) {
        if (temp_str.data[0] == '-') {
            is_negative = true;
            consumed_prefix = 1;
        } else if (temp_str.data[0] == '+') {
            consumed_prefix = 1;
        }
    }

    // String with only a sign, or an empty string for unsigned, is not a valid number.
    if (temp_str.length <= consumed_prefix) {
        return S2I_NOT_FOUND;
    }

    bool found_digit = false;
    T result = 0;

    size_t i = consumed_prefix;
    for (; i < temp_str.length; ++i) {
        char c = temp_str.data[i];
        if (c < '0' || c > '9') {
            break;
        }

        found_digit = true;
        uint8_t digit = c - '0';

        if (is_negative) {
            constexpr T min_val = std::numeric_limits<T>::min();
            if (result < min_val / 10 || (result == min_val / 10 && static_cast<T>(-digit) < min_val % 10)) {
                return S2I_OUT_OF_RANGE;
            }
            result = result * 10 - digit;
        } else {
            constexpr T max_val = std::numeric_limits<T>::max();
            if (result > max_val / 10 || (result == max_val / 10 && static_cast<T>(digit) > max_val % 10)) {
                return S2I_OUT_OF_RANGE;
            }
            result = result * 10 + digit;
        }
    }

    if (!found_digit) {
        return S2I_NOT_FOUND;
    }

    *value = result;
    string->data += i;
    string->length -= i;

    return S2I_OK;
}
