#include "cpputils/compact_memory_manager.h"
#include "cpputils/generic_cpu_allocator.h"
using namespace cpputils;

#undef NDEBUG
#include <assert.h>

static void TestAllocAndFree1() {
    const uint32_t block_bytes = 1024;
    GenericCpuAllocator ar;
    CompactMemoryManager mgr(&ar, block_bytes);

    uint32_t alloc_size = 100;
    auto ret = mgr.Alloc(alloc_size);
    assert(ret != nullptr);
    mgr.Free(ret, alloc_size);
    assert(mgr.GetAllocatedBytes() == block_bytes);

    alloc_size = block_bytes + 1;
    ret = mgr.Alloc(alloc_size);
    assert(ret != nullptr);
    mgr.Free(ret, alloc_size);
    assert(mgr.GetAllocatedBytes() == block_bytes * 3);
}

static void TestAllocAndFree2() {
    const uint32_t block_bytes = 1048576;
    GenericCpuAllocator ar;
    CompactMemoryManager mgr(&ar, block_bytes);

    uint32_t alloc_size = block_bytes - 1;
    auto ret = mgr.Alloc(alloc_size);
    assert(ret != nullptr);
    assert(mgr.GetAllocatedBytes() == block_bytes);
    mgr.Free(ret, alloc_size);

    // reuse last freed memories
    alloc_size = block_bytes - 2;
    ret = mgr.Alloc(alloc_size);
    assert(ret != nullptr);
    assert(mgr.GetAllocatedBytes() == block_bytes);
    // without mgr.Free()

    ret = mgr.Alloc(alloc_size);
    assert(ret != nullptr);
    assert(mgr.GetAllocatedBytes() == block_bytes * 2);
}

int main(void) {
    TestAllocAndFree1();
    TestAllocAndFree2();
    return 0;
}
