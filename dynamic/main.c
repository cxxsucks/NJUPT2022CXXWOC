#include <stdio.h>
#include "foo.h"

int main() {
    // Initialize a Fibonacci sequence
    fib_init(1, 1);
    do {
        printf("%u: %llu\n", fib_index(), fib_current());
    } while (fib_next());
    printf("%u values\n", fib_index() + 1);
    return 0;
}