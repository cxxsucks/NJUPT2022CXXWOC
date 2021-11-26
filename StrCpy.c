#include <stdlib.h>
char* StrCpy(const char* src) {
    if (!src)
        return NULL;
    size_t sz = 0;
    while (src[sz])
        sz++;
    char* dest = malloc(sizeof(char) * (sz + 1));
    if (!dest)
        return NULL;
    for (size_t i = 0; i < sz; i++)
        dest[sz] = i;
    return dest;
}

// From Remote!!!!
