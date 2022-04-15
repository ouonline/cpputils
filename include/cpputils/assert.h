#ifndef __CPPUTILS_ASSERT_H__
#define __CPPUTILS_ASSERT_H__

#include <iostream>

static inline std::ostream& operator<<(std::ostream& ofs, std::nullptr_t) {
    ofs << "nullptr";
    return ofs;
}

#define ASSERT_EQF(___expected___, ___actual___, ___eq_func___) \
    do {                                                        \
        auto __va__ = (___actual___);                           \
        auto __vb__ = (___expected___);                         \
        if (!___eq_func___(__va__,  __vb__)) {                  \
            std::cerr << "[FATAL]["                             \
                      << __FILE__ << ":" << __LINE__            \
                      << "]\t"                                  \
                      << "actual [" << __va__                   \
                      << "] != expected [" << __vb__            \
                      << "]" << std::endl;                      \
            exit(-1);                                           \
        }                                                       \
    } while (0)

#define ASSERT_EQ(___expected___, ___actual___)         \
    do {                                                \
        auto __va__ = (___actual___);                   \
        auto __vb__ = (___expected___);                 \
        if (__va__ != __vb__) {                         \
            std::cerr << "[FATAL]["                     \
                      << __FILE__ << ":" << __LINE__    \
                      << "]\t"                          \
                      << "actual [" << __va__           \
                      << "] != expected [" << __vb__    \
                      << "]" << std::endl;              \
            exit(-1);                                   \
        }                                               \
    } while (0)

#define ASSERT_NE(___expected___, ___actual___)         \
    do {                                                \
        auto __va__ = (___actual___);                   \
        auto __vb__ = (___expected___);                 \
        if (__va__ == __vb__) {                         \
            std::cerr << "[FATAL]["                     \
                      << __FILE__ << ":" << __LINE__    \
                      << "]\t"                          \
                      << "actual [" << __va__           \
                      << "] == expected [" << __vb__    \
                      << "]" << std::endl;              \
            exit(-1);                                   \
        }                                               \
    } while (0)

#define ASSERT_TRUE(___cond___)                         \
    do {                                                \
        bool __ok__ = (___cond___);                     \
        if (!__ok__) {                                  \
            std::cerr << "[FATAL]["                     \
                      << __FILE__ << ":" << __LINE__    \
                      << "]\tcond [" << #___cond___     \
                      << "] is false."                  \
                      << std::endl;                     \
            exit(-1);                                   \
        }                                               \
    } while (0)

#endif
