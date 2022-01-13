#ifndef __CPPUTILS_TIME_UTILS_H__
#define __CPPUTILS_TIME_UTILS_H__

#include <stdint.h>
#include <sys/time.h>

namespace outils {

static inline uint64_t DiffTimeUsec(struct timeval end,
                                    const struct timeval& begin) {
    if (end.tv_usec < begin.tv_usec) {
        --end.tv_sec;
        end.tv_usec += 1000000;
    }
    return (end.tv_sec - begin.tv_sec) * 1000000 +
        (end.tv_usec - begin.tv_usec);
}

}

#endif
