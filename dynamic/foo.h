#pragma once
#include <stdbool.h>

#ifndef _WIN32
#define FOO_API
#else
#ifdef FOO_EXPORTS
#define FOO_API __declspec(dllexport)
#else
#define FOO_API __declspec(dllimport)
#endif // FOO_EXPORTS
#endif // _WIN32

#ifdef __cplusplus
extern "C" {
#endif
/*! @brief Initialize a Fibonacci relation
 @param lhs F(0) @param rhs F(1)
 @warning This function must be called prior to the other three */
FOO_API void fib_init(unsigned long long lhs, unsigned long long rhs);
/*! @brief Produce the next fibonacci pair
 @return true on success, false on oveflow */
FOO_API bool fib_next();
//! @return current value
FOO_API unsigned long long fib_current();
//! @return current index
FOO_API unsigned fib_index();

#ifdef __cplusplus
}
#endif