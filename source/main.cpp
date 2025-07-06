#include <stdio.h>
#include <stdbool.h>

#define STR_IMPLEMENT
#include "str.hpp"

int main() {
    str a = str("alex");
    str b = str("alex");

    printf("Hello " PRI_str "!\n", FMT_str(a));
    str_debug_fprint(a, stdout);
    printf("Equal %d!\n", a == b);
    printf("a[0] = %c!\n", a[0]);
    return 0;
}
