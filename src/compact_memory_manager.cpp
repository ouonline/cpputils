#include "cpputils/compact_memory_manager.h"
using namespace std;

namespace cpputils {

CompactMemoryManager::CompactMemoryManager(Allocator* ar, uint64_t block_bytes)
    : m_block_bytes(block_bytes), m_allocator(ar), m_allocated_bytes(0) {
    m_blocks.reserve(64);
}

CompactMemoryManager::~CompactMemoryManager() {
    for (auto x = m_blocks.begin(); x != m_blocks.end(); ++x) {
        m_allocator->Free(*x);
    }
}

void CompactMemoryManager::Clear() {
    for (auto x = m_blocks.begin(); x != m_blocks.end(); ++x) {
        m_allocator->Free(*x);
    }

    m_allocated_bytes = 0;
    m_addr2bytes.clear();
    m_bytes2addr.clear();
    m_blocks.clear();
}

static void RemoveFromBytes2Addr(void* addr, uint64_t bytes, map<uint64_t, set<void*>>* bytes2addr) {
    auto b2a_iter = bytes2addr->find(bytes);
    if (b2a_iter != bytes2addr->end()) {
        b2a_iter->second.erase(addr);
        if (b2a_iter->second.empty()) {
            bytes2addr->erase(b2a_iter);
        }
    }
}

static void AddFreeBlock(void* new_addr, uint64_t new_bytes, map<uint64_t, set<void*>>* bytes2addr,
                         map<void*, uint64_t>* addr2bytes) {
    auto ret_pair = bytes2addr->insert(make_pair(new_bytes, set<void*>()));
    ret_pair.first->second.insert(new_addr);
    addr2bytes->insert(make_pair(new_addr, new_bytes));
}

static inline uint64_t Align(uint64_t x, uint64_t n) {
    return (x + n - 1) & (~(n - 1));
}

void* CompactMemoryManager::Alloc(uint64_t bytes_needed) {
    // find a best-fit bytes block in free blocks
    auto b2a_iter = m_bytes2addr.lower_bound(bytes_needed);
    if (b2a_iter == m_bytes2addr.end()) {
        // create a new block if not found
        uint64_t bytes_allocated = Align(bytes_needed, m_block_bytes);
        auto new_block = m_allocator->Alloc(bytes_allocated);
        if (!new_block) {
            return nullptr;
        }
        m_allocated_bytes += bytes_allocated;
        m_blocks.push_back(new_block);

        if (bytes_needed < bytes_allocated) {
            AddFreeBlock((char*)new_block + bytes_needed, bytes_allocated - bytes_needed, &m_bytes2addr, &m_addr2bytes);
        }

        return new_block;
    }

    // if best-fit bytes block(s) are found, use the first one and remove it from free list
    auto addr_iter = b2a_iter->second.begin();
    auto res_addr = *addr_iter;
    b2a_iter->second.erase(addr_iter);
    if (b2a_iter->second.empty()) {
        m_bytes2addr.erase(b2a_iter);
    }

    // find size of the chosen block and remove it from m_addr2bytes
    auto a2b_iter = m_addr2bytes.find(res_addr);

    // insert the rest of block into free list
    if (a2b_iter->second > bytes_needed) {
        AddFreeBlock((char*)res_addr + bytes_needed, a2b_iter->second - bytes_needed, &m_bytes2addr, &m_addr2bytes);
    }

    m_addr2bytes.erase(a2b_iter);

    return res_addr;
}

void CompactMemoryManager::Free(void* addr, uint64_t bytes) {
    void* next_addr = (char*)addr + bytes;

    // find and merge with its successor
    auto a2b_iter = m_addr2bytes.find(next_addr);
    if (a2b_iter != m_addr2bytes.end()) {
        bytes += a2b_iter->second;
        RemoveFromBytes2Addr(next_addr, a2b_iter->second, &m_bytes2addr);
        m_addr2bytes.erase(a2b_iter);
    }

    // find and merge with its predecessor
    a2b_iter = m_addr2bytes.lower_bound(addr);
    if (a2b_iter == m_addr2bytes.begin()) {
        a2b_iter = m_addr2bytes.end();
    } else {
        --a2b_iter;
    }

    if (a2b_iter != m_addr2bytes.end() && (char*)a2b_iter->first + a2b_iter->second == addr) {
        RemoveFromBytes2Addr(a2b_iter->first, a2b_iter->second, &m_bytes2addr);
        a2b_iter->second += bytes;

        auto ret_pair = m_bytes2addr.insert(make_pair(a2b_iter->second, set<void*>()));
        ret_pair.first->second.insert(a2b_iter->first);
    } else {
        AddFreeBlock(addr, bytes, &m_bytes2addr, &m_addr2bytes);
    }
}

}
