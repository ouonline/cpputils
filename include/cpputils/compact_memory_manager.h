#ifndef __CPPUTILS_COMPACT_MEMORY_MANAGER_H__
#define __CPPUTILS_COMPACT_MEMORY_MANAGER_H__

#include "cpputils/allocator.h"
#include <vector>
#include <map>
#include <set>

namespace cpputils {

class CompactMemoryManager final {
public:
    /** @param block_bytes MUST be power of 2 */
    CompactMemoryManager(Allocator* ar, uint64_t block_bytes = 1048576);
    ~CompactMemoryManager();

    void* Alloc(uint64_t bytes);
    void Free(void* addr, uint64_t bytes);

    uint64_t GetAllocatedBytes() const {
        return m_allocated_bytes;
    }

private:
    void Clear();

private:
    const uint64_t m_block_bytes;
    Allocator* m_allocator;
    uint64_t m_allocated_bytes;
    std::map<void*, uint64_t> m_addr2bytes;
    std::map<uint64_t, std::set<void*>> m_bytes2addr;
    std::vector<void*> m_blocks;
};

}

#endif
