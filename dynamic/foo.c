#include "foo.h"
// Dynamic Library Internal State Globals
static unsigned long long fib_prev_;
static unsigned long long fib_cur_;
unsigned fib_idx_;

#ifndef __UINT64_MAX__
#define __UINT64_MAX__ (~0ull)
#endif
#ifndef __UINT32_MAX__
#define __UINT32_MAX__ (~0u)
#endif
FOO_API void fib_init(unsigned long long lhs, unsigned long long rhs) {
    fib_idx_ = 0;
    fib_cur_ = lhs;
    fib_prev_ = rhs;
}

FOO_API bool fib_next() {
    // CHECK overflow before calculation
    if (__UINT64_MAX__ - fib_prev_ < fib_cur_ || 
            fib_idx_ == __UINT32_MAX__) 
        return false;

    // the 1st value is fib_cur; no need to add
    if (fib_idx_ != 0)
        fib_prev_ += fib_cur_;
    // swap prev and cur
    fib_prev_ ^= fib_cur_;
    fib_cur_  ^= fib_prev_;
    fib_prev_ ^= fib_cur_;
    fib_idx_++;
    return true;
}

FOO_API unsigned long long fib_current() {return fib_cur_;}
FOO_API unsigned fib_index() {return fib_idx_;}
