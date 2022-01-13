#ifndef __CPPUTILS_ASSERT_H__
#define __CPPUTILS_ASSERT_H__

#include <iostream>

#define ASSERT_EQF(___actual___, ___expected___, ___eq_func___) \
    do {                                                        \
        auto __va__ = (___actual___);                           \
        auto __vb__ = (___expected___);                         \
        if (!___eq_func___(__va__,  __vb__)) {                  \
            std::cerr << "[FATAL] "                             \
                      << __FILE__ << ":" << __LINE__            \
                      << ":\t"                                  \
                      << "actual [" << __va__                   \
                      << "] != expected [" << __vb__            \
                      << "]" << std::endl;                      \
            exit(-1);                                           \
        }                                                       \
    } while (0)

#define ASSERT_EQ(___actual___, ___expected___)         \
    do {                                                \
        auto __va__ = (___actual___);                   \
        auto __vb__ = (___expected___);                 \
        if (__va__ != __vb__) {                         \
            std::cerr << "[FATAL] "                     \
                      << __FILE__ << ":" << __LINE__    \
                      << ":\t"                          \
                      << "actual [" << __va__           \
                      << "] != expected [" << __vb__    \
                      << "]" << std::endl;              \
            exit(-1);                                   \
        }                                               \
    } while (0)

#define ASSERT_TRUE(___cond___)                         \
    do {                                                \
        bool __ok__ = (___cond___);                     \
        if (!__ok__) {                                  \
            std::cerr << "[FATAL] "                     \
                      << __FILE__ << ":" << __LINE__    \
                      << ":\tcond [" << #___cond___     \
                      << "] is false."                  \
                      << std::endl;                     \
            exit(-1);                                   \
        }                                               \
    } while (0)

#endif
