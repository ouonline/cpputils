#ifndef __CPPUTILS_GENERIC_CPU_ALLOCATOR_H__
#define __CPPUTILS_GENERIC_CPU_ALLOCATOR_H__

#include <cstdlib>
#include "cpputils/allocator.h"

namespace outils {

class GenericCpuAllocator : public Allocator {
public:
    void* Alloc(uint64_t bytes) override { return ::malloc(bytes); }
    void Free(void* ptr) override { ::free(ptr); }
};

}

#endif
