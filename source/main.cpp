#include <stdio.h>
#include <stdbool.h>
#include "types.h"

#define STR_IMPLEMENT
#include "str.hpp"

#include "str_to_int.hpp"

#include "hprint.hpp"
using namespace hp;

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
    /*auto d_r = str_to_int(d, &d_i);*/
    /*auto e_r = str_to_int(e, &e_i);*/
    /*auto f_r = str_to_int(f, &f_i);*/

    print("% -> % (%)\n", a, a_i, (u8)a_r);
    print("% -> % (%)\n", b, b_i, (u8)b_r);
    print("% -> % (%)\n", c, c_i, (u8)c_r);
    /*print("% -> % (%)\n", d, d_i, (u8)d_r);*/
    /*print("% -> % (%)\n", e, e_i, (u8)e_r);*/
    /*print("% -> % (%)\n", f, f_i, (u8)f_r);*/
    return 0;
}
