#include "cpputils/compact_addr_manager.h"
using namespace cpputils;

#undef NDEBUG
#include <assert.h>
#include <vector>
using namespace std;

class TestAllocator final : public CompactAddrManager::Allocator {
public:
    TestAllocator() : m_allocated_size(0) {}
    ~TestAllocator() {
        for (auto x = m_chunks.begin(); x != m_chunks.end(); ++x) {
            free(*x);
        }
    }
    uint64_t GetAllocatedSize() const override {
        return m_allocated_size;
    }
    pair<uintptr_t, uint64_t> Alloc(uint64_t needed) override {
        auto ptr = malloc(needed);
        if (!ptr) {
            return pair<uintptr_t, uint64_t>(UINTPTR_MAX, 0);
        }
        m_allocated_size += needed;
        m_chunks.push_back(ptr);
        return pair<uintptr_t, uint64_t>((uintptr_t)ptr, needed);
    }

private:
    uint64_t m_allocated_size;
    vector<void*> m_chunks;
};

static void TestAllocAndFree1() {
    TestAllocator ar;
    CompactAddrManager mgr(&ar);

    uint32_t alloc_size = 100;
    auto ret = mgr.Alloc(alloc_size);
    assert(ret != UINTPTR_MAX);
    mgr.Free(ret, alloc_size);
    assert(ar.GetAllocatedSize() == alloc_size);

    auto alloc_size2 = alloc_size + 1;
    ret = mgr.Alloc(alloc_size2);
    assert(ret != UINTPTR_MAX);
    mgr.Free(ret, alloc_size2);
    assert(ar.GetAllocatedSize() == alloc_size + alloc_size2);
}

static void TestAllocAndFree2() {
    TestAllocator ar;
    CompactAddrManager mgr(&ar);

    uint32_t alloc_size = 100;
    auto ret = mgr.Alloc(alloc_size);
    assert(ret != UINTPTR_MAX);
    assert(ar.GetAllocatedSize() == alloc_size);
    mgr.Free(ret, alloc_size);

    // reuse last freed memories
    auto alloc_size2 = alloc_size - 1;
    ret = mgr.Alloc(alloc_size2);
    assert(ret != UINTPTR_MAX);
    assert(ar.GetAllocatedSize() == alloc_size);
    // without mgr.Free()

    ret = mgr.Alloc(alloc_size);
    assert(ret != UINTPTR_MAX);
    assert(ar.GetAllocatedSize() == alloc_size + alloc_size);
}

int main(void) {
    TestAllocAndFree1();
    TestAllocAndFree2();
    return 0;
}
