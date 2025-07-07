#include <stdio.h>
#include <stdbool.h>
#include "types.h"

#define STR_IMPLEMENT
#include "str.hpp"

#include "str_to_int.hpp"

#include "hprint.hpp"
using namespace hprint;

int main() {
    str a = str("alex");
    str b = str("255");
    str c = str("256");
    str d = str("256");
    str e = str("-10");
    str f = str("-127");

    u8 a_i, b_i, c_i; s16 d_i; s8 e_i, f_i;
    auto a_r = str_to_int(a, &a_i);
    auto b_r = str_to_int(b, &b_i);
    auto c_r = str_to_int(c, &c_i);
    auto d_r = str_to_int(d, &d_i);
    auto e_r = str_to_int(e, &e_i);
    auto f_r = str_to_int(f, &f_i);

    print("" PRI_str " -> " PRI_u8 " (" PRI_u8 ")\n", FMT_str(a), a_i, a_r);
    print("" PRI_str " -> " PRI_u8 " (" PRI_u8 ")\n", FMT_str(b), b_i, b_r);
    print("" PRI_str " -> " PRI_u8 " (" PRI_u8 ")\n", FMT_str(c), c_i, c_r);
    print("" PRI_str " -> " PRI_s16 " (" PRI_s16 ")\n", FMT_str(d), d_i, d_r);
    print("" PRI_str " -> " PRI_s8 " (" PRI_s8 ")\n", FMT_str(e), e_i, e_r);
    print("" PRI_str " -> " PRI_s8 " (" PRI_s8 ")\n", FMT_str(f), f_i, f_r);
    return 0;
}
