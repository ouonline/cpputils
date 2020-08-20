#ifndef __CPPUTILS_DEFAULT_ALLOCATOR_H__
#define __CPPUTILS_DEFAULT_ALLOCATOR_H__

#include <stdint.h>
#include <cstdlib>

namespace outils {

class DefaultAllocator final {
public:
    void* Alloc(uint64_t size) {
        return malloc(size);
    }
    void Free(void* ptr) {
        free(ptr);
    }
};

}

#endif
