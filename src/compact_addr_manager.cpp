#include "cpputils/compact_addr_manager.h"
using namespace std;

namespace cpputils {

static void RemoveFromSize2Addr(uintptr_t addr, uint64_t size,
                                map<uint64_t, set<uintptr_t>>* size2addr) {
    auto s2a_iter = size2addr->find(size);
    if (s2a_iter != size2addr->end()) {
        s2a_iter->second.erase(addr);
        if (s2a_iter->second.empty()) {
            size2addr->erase(s2a_iter);
        }
    }
}

static void AddFreeBlock(uintptr_t new_addr, uint64_t new_size,
                         map<uint64_t, set<uintptr_t>>* size2addr,
                         map<uintptr_t, uint64_t>* addr2size) {
    auto ret_pair = size2addr->insert(make_pair(new_size, set<uintptr_t>()));
    ret_pair.first->second.insert(new_addr);
    addr2size->insert(make_pair(new_addr, new_size));
}

uintptr_t CompactAddrManager::AllocByAllocator(uint64_t needed) {
    auto alloc_res = m_ar->Alloc(needed);
    if (alloc_res.first == UINTPTR_MAX) {
        return UINTPTR_MAX;
    }

    // merge with the max-addr block if possible
    auto max_addr_iter = m_addr2size.rbegin();
    if ((max_addr_iter != m_addr2size.rend()) &&
        (max_addr_iter->first + max_addr_iter->second == alloc_res.first)) {
        auto new_addr = max_addr_iter->first;
        auto new_size = max_addr_iter->second + alloc_res.second;

        RemoveFromSize2Addr(max_addr_iter->first, max_addr_iter->second,
                            &m_size2addr);
        m_addr2size.erase((++max_addr_iter).base());
        AddFreeBlock(new_addr + needed, new_size - needed, &m_size2addr,
                     &m_addr2size);

        return new_addr;
    }

    if (needed < alloc_res.second) {
        AddFreeBlock(alloc_res.first + needed, alloc_res.second - needed,
                     &m_size2addr, &m_addr2size);
    }

    return alloc_res.first;
}

uintptr_t CompactAddrManager::AllocByVMAllocator(uint64_t needed) {
    auto end_addr = m_vmr->GetReservedBase() + m_vmr->GetAllocatedSize();
    auto ret_addr = end_addr;

    // finds the largest free address to see whether we can allocate from the
    // end of allocated area
    auto max_addr_iter = m_addr2size.rbegin();
    bool is_consecutive =
        (max_addr_iter != m_addr2size.rend() &&
         (max_addr_iter->first + max_addr_iter->second == end_addr));
    if (is_consecutive) {
        ret_addr = max_addr_iter->first;
        needed -= max_addr_iter->second;
    }

    uint64_t allocated = m_vmr->Extend(needed);
    if (allocated == 0) {
        return UINTPTR_MAX;
    }

    // removes the previous largest addr block because it is merged with the
    // newly allocated one
    if (is_consecutive) {
        RemoveFromSize2Addr(max_addr_iter->first, max_addr_iter->second,
                            &m_size2addr);
        m_addr2size.erase((++max_addr_iter).base());
    }

    if (needed < allocated) {
        AddFreeBlock(end_addr + needed, allocated - needed, &m_size2addr,
                     &m_addr2size);
    }

    return ret_addr;
}

uintptr_t CompactAddrManager::Alloc(uint64_t needed) {
    // find a best-fit bytes block in free blocks
    auto s2a_iter = m_size2addr.lower_bound(needed);
    if (s2a_iter == m_size2addr.end()) {
        if (m_ar) {
            return AllocByAllocator(needed);
        }
        return AllocByVMAllocator(needed);
    }

    // if best-fit bytes block(s) are found, use the first one and remove it
    // from free list
    auto addr_iter = s2a_iter->second.begin();
    auto res_addr = *addr_iter;
    s2a_iter->second.erase(addr_iter);
    if (s2a_iter->second.empty()) {
        m_size2addr.erase(s2a_iter);
    }

    // find size of the chosen block and remove it from m_addr2size
    auto a2s_iter = m_addr2size.find(res_addr);
    auto block_rest_size = a2s_iter->second;
    m_addr2size.erase(a2s_iter);

    // insert the rest of block into free list
    if (block_rest_size > needed) {
        AddFreeBlock(res_addr + needed, block_rest_size - needed, &m_size2addr,
                     &m_addr2size);
    }

    return res_addr;
}

void CompactAddrManager::Free(uintptr_t addr, uint64_t size) {
    auto next_addr = addr + size;

    // find and merge with its successor
    auto a2s_iter = m_addr2size.find(next_addr);
    if (a2s_iter != m_addr2size.end()) {
        size += a2s_iter->second;
        RemoveFromSize2Addr(next_addr, a2s_iter->second, &m_size2addr);
        m_addr2size.erase(a2s_iter);
    }

    // find and merge with its predecessor
    a2s_iter = m_addr2size.lower_bound(addr);
    if (a2s_iter == m_addr2size.begin()) {
        a2s_iter = m_addr2size.end();
    } else {
        --a2s_iter;
    }

    if (a2s_iter != m_addr2size.end() &&
        a2s_iter->first + a2s_iter->second == addr) {
        RemoveFromSize2Addr(a2s_iter->first, a2s_iter->second, &m_size2addr);
        a2s_iter->second += size;

        auto ret_pair =
            m_size2addr.insert(make_pair(a2s_iter->second, set<uintptr_t>()));
        ret_pair.first->second.insert(a2s_iter->first);
    } else {
        AddFreeBlock(addr, size, &m_size2addr, &m_addr2size);
    }
}

}
