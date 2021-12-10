#include "foo.h"
#include <stdio.h>

int main() {
    double a = 20, b = 22;
    printf("a + b = %lf\na - b = %lf\na * b = %lf\na / b = %lf\n",
        mymath_add(a, b), mymath_sub(a, b),
        mymath_mult(a, b), mymath_divi(a, b)
    );
    return 0;
}