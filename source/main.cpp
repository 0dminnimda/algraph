#include <stdio.h>
#include <stdbool.h>
#include "types.h"

#include <stdio.h>
#define STR_IMPLEMENT
#include "array.hpp"
#include "print.hpp"

int main() {
    Array<int> my_ints = {};

    print("adding elements...\n");
    array_add(&my_ints, 10);
    array_add(&my_ints, 20);
    array_add(&my_ints, 30);

    print("Array has % elements, capacity for %\n", my_ints.length, my_ints.capacity);
    print("Element at index 1: %\n", my_ints[1]);

    int new_data[] = {4, 5, 6, 7, 8, 9, 10};
    array_add_range(&my_ints, new_data, 7);
    print("Array has % elements, capacity for %\n", my_ints.length, my_ints.capacity);

    print("Iterating with range-based for loop:\n");
    for (int val : my_ints) {
        printf("  - %d\n", val);
    }

    int popped_value = array_pop(&my_ints);
    print("Popped value: %\n", popped_value);
    print("Array now has % elements %\n", my_ints.length, 1);

    array_free(&my_ints);
    print("Array freed. Length is now %-%\n", my_ints.length, str("hello"));

    print("\nCreating an array with a macro:\n");
    Array<float> my_floats = array_of(1.1f, 2.2f, 3.3456f, 4.4f);

    for (float f : my_floats) {
        print("  - %\n", f);
    }

    array_free(&my_floats);

    return 0;
}

/*#define STR_IMPLEMENT*/
/*#include "str.hpp"*/
/**/
/*#include "str_to_int.hpp"*/
/**/
/*#include "hprint.hpp"*/
/*using namespace hp;*/
/**/
/*int main() {*/
/*    str a = str("alex");*/
/*    str b = str("255");*/
/*    str c = str("256");*/
/*    str d = str("256");*/
/*    str e = str("-10");*/
/*    str f = str("-127");*/
/**/
/*    u8 a_i, b_i, c_i; s16 d_i; s8 e_i, f_i;*/
/*    auto a_r = str_to_int(a, &a_i);*/
/*    auto b_r = str_to_int(b, &b_i);*/
/*    auto c_r = str_to_int(c, &c_i);*/
/*    / *auto d_r = str_to_int(d, &d_i);* /*/
/*    / *auto e_r = str_to_int(e, &e_i);* /*/
/*    / *auto f_r = str_to_int(f, &f_i);* /*/
/**/
/*    print("% -> % (%)\n", a, a_i, (u8)a_r);*/
/*    print("% -> % (%)\n", b, b_i, (u8)b_r);*/
/*    print("% -> % (%)\n", c, c_i, (u8)c_r);*/
/*    / *print("% -> % (%)\n", d, d_i, (u8)d_r);* /*/
/*    / *print("% -> % (%)\n", e, e_i, (u8)e_r);* /*/
/*    / *print("% -> % (%)\n", f, f_i, (u8)f_r);* /*/
/*    return 0;*/
/*}*/
