#ifndef __CPPUTILS_COMPACT_ADDR_MANAGER_H__
#define __CPPUTILS_COMPACT_ADDR_MANAGER_H__

#include <stdint.h>
#include <map>
#include <set>

namespace cpputils {

class CompactAddrManager final {
public:
    class Allocator {
    public:
        virtual ~Allocator() {}
        virtual uint64_t GetAllocatedSize() const = 0;
        /** returns the starting addr and size allocated, or <UINTPTR_MAX,
         * undefined> if failed. */
        virtual std::pair<uintptr_t, uint64_t> Alloc(uint64_t needed) = 0;
    };

    class VMAllocator {
    public:
        virtual ~VMAllocator() {}
        virtual uintptr_t GetReservedBase() const = 0;
        virtual uint64_t GetAllocatedSize() const = 0;
        /** acquires `needed` from the end position and returns the actual size
         * allocated or 0 if failed. */
        virtual uint64_t Extend(uint64_t needed) = 0;
    };

public:
    CompactAddrManager(Allocator* ar) : m_ar(ar) {}
    CompactAddrManager(VMAllocator* mgr) : m_vmr(mgr) {}

    /** returns UINTPTR_MAX if failed. */
    uintptr_t Alloc(uint64_t size);
    void Free(uintptr_t addr, uint64_t size);

private:
    /** returns UINTPTR_MAX if failed. */
    uintptr_t AllocByAllocator(uint64_t needed);
    uintptr_t AllocByVMAllocator(uint64_t needed);

private:
    Allocator* m_ar = nullptr;
    VMAllocator* m_vmr = nullptr;
    std::map<uintptr_t, uint64_t> m_addr2size;
    std::map<uint64_t, std::set<uintptr_t>> m_size2addr;

private:
    CompactAddrManager(const CompactAddrManager&) = delete;
    CompactAddrManager& operator=(const CompactAddrManager&) = delete;
};

}

#endif
