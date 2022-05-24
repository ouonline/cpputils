#include "cpputils/compact_memory_manager.h"
#include "cpputils/generic_cpu_allocator.h"
#include "cpputils/assert.h"
using namespace cpputils;

static void TestAllocAndFree1() {
    const uint32_t block_bytes = 1024;
    GenericCpuAllocator ar;
    CompactMemoryManager mgr(&ar, block_bytes);

    uint32_t alloc_size = 100;
    auto ret = mgr.Alloc(alloc_size);
    ASSERT_NE(nullptr, ret);
    mgr.Free(ret, alloc_size);
    ASSERT_EQ(block_bytes, mgr.GetAllocatedBytes());

    alloc_size = block_bytes + 1;
    ret = mgr.Alloc(alloc_size);
    ASSERT_NE(nullptr, ret);
    mgr.Free(ret, alloc_size);
    ASSERT_EQ(block_bytes * 3, mgr.GetAllocatedBytes());
}

static void TestAllocAndFree2() {
    const uint32_t block_bytes = 1048576;
    GenericCpuAllocator ar;
    CompactMemoryManager mgr(&ar, block_bytes);

    uint32_t alloc_size = block_bytes - 1;
    auto ret = mgr.Alloc(alloc_size);
    ASSERT_NE(nullptr, ret);
    ASSERT_EQ(block_bytes, mgr.GetAllocatedBytes());
    mgr.Free(ret, alloc_size);

    // reuse last freed memories
    alloc_size = block_bytes - 2;
    ret = mgr.Alloc(alloc_size);
    ASSERT_NE(nullptr, ret);
    ASSERT_EQ(block_bytes, mgr.GetAllocatedBytes());
    // without mgr.Free()

    ret = mgr.Alloc(alloc_size);
    ASSERT_NE(nullptr, ret);
    ASSERT_EQ(block_bytes * 2, mgr.GetAllocatedBytes());
}

int main(void) {
    TestAllocAndFree1();
    TestAllocAndFree2();
    return 0;
}
