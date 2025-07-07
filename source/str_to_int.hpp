/* Treats as much characters as digits as it can.
 * Makes a slice to the string after the parsed integer. */

#include <type_traits>
#include <limits>
#include "checked_int.h"

template <typename T>
constexpr size_t int_bit_width() {
    static_assert(std::is_integral<T>::value, "T must be an integer type");
    return std::numeric_limits<T>::digits + (std::numeric_limits<T>::is_signed ? 1 : 0);
}

template <typename T>
constexpr size_t int_decimal_digits() {
    static_assert(std::is_integral<T>::value, "T must be an integer type");
    int digits = 1;
    T n = std::numeric_limits<T>::max();
    while (n >= 10) {
        n /= 10;
        digits++;
    }
    return digits;
}

template <typename T>
constexpr u8 int_most_significant_digit() {
    static_assert(std::is_integral<T>::value, "T must be an integer type");
    T n = std::numeric_limits<T>::max();
    while (n >= 10) {
        n /= 10;
    }
    return n;
}


static_assert(int_bit_width<uint16_t>() == 16);
static_assert(int_decimal_digits<uint8_t>() == 3);
static_assert(int_most_significant_digit<int16_t>() == 3);

enum S2I_Result {
    S2I_OK,
    S2I_NOT_FOUND,
    S2I_OUT_OF_RANGE
};

/* TODO: ignore leading zeros,
 * TODO: except leading 0b, 0o and 0x.

 * TODO: Allow to pass in a target base.

 * TODO: Allow to have leading + or -
 * (no spaces between them and number allowed).

 * TODO: Allow single underscores in the number
 * (except the first, last and characteres right after 0b,0o,0x).
 * Not allowed: _234, 456_, 0x_F, 0xF_
 * Allowed: 2_3_4, 0xF_A
 */

/* SEE:
 * https://github.com/lattera/glibc/blob/895ef79e04a953cac1493863bcae29ad85657ee1/stdlib/strtol_l.c
 * https://github.com/python/cpython/blob/6557af669899f18f8d123f8e1b6c3380d502c519/Objects/longobject.c#L2518
 */

/* The basic idea is that given a spefic type, you know it's maximum/minimum.
 * If you leave out the most significant digit (MSD) (make number out of one less digits then maximum)
 * then you can guarantee, there were no overflows while you are constructing it.
 * The only possible overflow is when you add MSD to the rest.

 * For example, for u8 all numbers from 0 to 99 are guranteed to fit,
 * but if you start parting 299, thus adding 200 and 99,
 * then you will need to check for overflow,
 * which ofter should be a single instruction and a jump.
 */

template <typename T>
S2I_Result str_to_int(str string, T *value) {
    str temporary = string;
    return str_to_int_and_consume(&temporary, value);
}

template <typename T>
S2I_Result str_to_int_and_consume(str *string, T *value) {
    static_assert(std::is_signed<T>::value, "parsing of unsigned ints is not implemented");

    if (string->length == 0) return S2I_NOT_FOUND;
    char c = string->data[0];

    u8 msd;
    if ('0' <= c && c <= '9') {
        msd = c - '0';
    } else {
        return S2I_NOT_FOUND;
    }

    T msd_multiplier = 1;
    T rest = 0;
    constexpr u8 max_digits = int_decimal_digits<T>();
    u8 count = 1;
    for (; count < max_digits && count < string->length; ++count) {
        c = string->data[count];
        if ('0' <= c && c <= '9') {
            msd_multiplier *= 10;
            rest = rest * 10 + (c - '0');
        } else {
            break;
        }
    }

    if (count != max_digits) {
        *value = msd * msd_multiplier + rest;
        string->data += count;
        string->length -= count;
        return S2I_OK;
    }

    if (count < string->length) {
        c = string->data[count];
        if ('0' <= c && c <= '9') {
            /* Got more than max digits, definitely not in range. */
            return S2I_OUT_OF_RANGE;
        }
    }

    constexpr u8 max_value_for_leading_digit
        = int_most_significant_digit<T>();

    if (msd > max_value_for_leading_digit) {
        /* Most significant digit value exceeded it's limit */
        return S2I_OUT_OF_RANGE;
    }
    msd_multiplier *= msd;

    if (ckd_add(value, msd_multiplier, rest)) {
        return S2I_OUT_OF_RANGE;
    }
    string->data += count;
    string->length -= count;
    return S2I_OK;
}

