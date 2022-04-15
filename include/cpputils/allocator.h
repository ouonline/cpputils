#ifndef __CPPUTILS_ALLOCATOR_H__
#define __CPPUTILS_ALLOCATOR_H__

#include <stdint.h>

namespace outils {

class Allocator {
public:
    virtual ~Allocator() {}
    virtual void* Alloc(uint64_t bytes) = 0;
    virtual void Free(void* ptr) = 0;
};

}

#endif
