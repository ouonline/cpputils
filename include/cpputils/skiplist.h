#ifndef __CPPUTILS_SKIPLIST_H__
#define __CPPUTILS_SKIPLIST_H__

#include "generic_cpu_allocator.h"
#include "cutils/random/xoshiro256ss.h"
#include <cstdint>
#include <cstring>
#include <utility>

namespace cpputils {

static constexpr uint32_t SKIPLIST_DIFF_LT = 0;
static constexpr uint32_t SKIPLIST_DIFF_EQ = 1;
static constexpr uint32_t SKIPLIST_DIFF_GT = 2;
static constexpr uint32_t SKIPLIST_DIFF_GE = (SKIPLIST_DIFF_EQ | SKIPLIST_DIFF_GT);

/*
  `Comparator` has the form of `uint32_t func(const Key& a, const Key& b)`, which returns
  `SKIPLIST_DIFF_LT`, `SKIPLIST_DIFF_EQ`, `SKIPLIST_DIFF_GT` for a < b, a == b, a > b, respectively.
*/
template <typename Key, typename Value, typename Comparator,
          typename GetKeyFromValue, typename Allocator>
class SkipList final : public Allocator {
private:
    static constexpr uint32_t MAX_LEVEL = 12;

private:
    struct DataNode final {
        uint32_t level;
        DataNode* forward[0];
    };

    struct HeadNode final {
        uint32_t level;
        DataNode* forward[MAX_LEVEL];
    };

public:
    class Iterator final {
    public:
        Value* operator-> () {
            return m_value;
        }
        const Value* operator-> () const {
            return m_value;
        }
        Value& operator* () {
            return *m_value;
        }
        const Value& operator* () const {
            return *m_value;
        }
        bool operator== (const Iterator& it) const {
            return (m_node == it.m_node);
        }
        bool operator!= (const Iterator& it) const {
            return (m_node != it.m_node);
        }
        void operator++ () {
            m_node = m_node->forward[0];
            if (m_node) {
                m_value = GetValueFromNode(m_node);
            }
        }

    private:
        friend class SkipList;
        Iterator(DataNode* node = nullptr) : m_node(node) {
            if (node) {
                m_value = GetValueFromNode(node);
            } else {
                m_value = nullptr;
            }
        }

    private:
        DataNode* m_node;
        Value* m_value;
    };

public:
    SkipList() {
        xoshiro256ss_init(&m_rand, (uintptr_t)this);
        memset(&m_head, 0, sizeof(HeadNode));
    }

    ~SkipList() {
        DoDestroy();
    }

    template <typename ValueType>
    std::pair<Iterator, bool> Insert(ValueType&& value) {
        const Key& key = m_get_key(value);

        uint32_t ge_diff = UINT32_MAX;
        DataNode* update[MAX_LEVEL];
        auto node = DoLookupGreaterEqual(key, &ge_diff, update);
        if (node && (ge_diff == SKIPLIST_DIFF_EQ)) {
            return std::pair<Iterator, bool>(Iterator(node), false);
        }

        node = DoInsert(std::forward<ValueType>(value), update);
        return std::pair<Iterator, bool>(Iterator(node), (node != nullptr));
    }

    template <typename ValueType = Value>
    bool Remove(const Key& key, ValueType* value = nullptr) {
        return DoRemove(key, value, SKIPLIST_DIFF_EQ);
    }

    template <typename ValueType = Value>
    bool RemoveGreaterEqual(const Key& key, ValueType* value = nullptr) {
        return DoRemove(key, value, SKIPLIST_DIFF_GE);
    }

    void Clear() {
        DoDestroy();
        memset(&m_head, 0, sizeof(HeadNode));
    }

    Iterator Lookup(const Key& key) const {
        uint32_t ge_diff = UINT32_MAX;
        auto node = DoLookupGreaterEqual(key, &ge_diff);
        if (node && (ge_diff == SKIPLIST_DIFF_EQ)) {
            return Iterator(node);
        }
        return Iterator();
    }

    Iterator LookupGreaterEqual(const Key& key) const {
        auto node = DoLookupGreaterEqual(key);
        return Iterator(node);
    }

    Iterator LookupLessThan(const Key& key) const {
        auto node = DoLookupLessThan(key);
        if (node == (DataNode*)(&m_head)) {
            return Iterator();
        }
        return Iterator(node);
    }

    bool IsEmpty() const {
        return (m_head.level == 0);
    }

    Iterator GetBeginIterator() const {
        return Iterator(m_head.forward[0]);
    }

    Iterator GetEndIterator() const {
        return Iterator();
    }

private:
    // ge_diff: the greater or equal diff value of the lastest comparison
    DataNode* DoLookupLessThan(const Key& key, uint32_t* ge_diff = nullptr, DataNode** update = nullptr) const {
        auto prev = (DataNode*)(&m_head);
        for (uint32_t l = prev->level; l > 0; --l) {
            const uint32_t level = l - 1;
            auto node = prev->forward[level];
            while (node) {
                auto pvalue = GetValueFromNode(node);
                uint32_t cur_diff = m_cmp(m_get_key(*pvalue), key);
                if (cur_diff & SKIPLIST_DIFF_GE) {
                    if (ge_diff) {
                        *ge_diff = cur_diff;
                    }
                    break;
                }
                prev = node;
                node = node->forward[level];
            }

            if (update) {
                update[level] = prev;
            }
        }

        return prev;
    }

    DataNode* DoLookupGreaterEqual(const Key& key, uint32_t* ge_diff = nullptr, DataNode** update = nullptr) const {
        auto node = DoLookupLessThan(key, ge_diff, update);
        return node->forward[0];
    }

    template <typename ValueType>
    bool DoRemove(const Key& key, ValueType* value, uint32_t ge_diff_mask) {
        uint32_t ge_diff = UINT32_MAX;
        DataNode* update[MAX_LEVEL];
        auto node = DoLookupGreaterEqual(key, &ge_diff, update);
        if (!node) {
            return false;
        }
        if (!(ge_diff & ge_diff_mask)) {
            return false;
        }

        for (uint32_t level = 0; level < m_head.level; ++level) {
            if (update[level]->forward[level] != node) {
                break;
            }
            update[level]->forward[level] = node->forward[level];
        }

        auto pvalue = GetValueFromNode(node);
        if (value) {
            *value = std::move(*pvalue);
        }

        pvalue->~Value();
        this->Free(pvalue);

        while (m_head.level > 0 && !m_head.forward[m_head.level - 1]) {
            --m_head.level;
        }

        return true;
    }

    template <typename ValueType>
    DataNode* DoInsert(ValueType&& value, DataNode* update[]) {
        const uint32_t level = GenRandomLevel();

        auto base = (char*)this->Alloc(sizeof(Value) +
                                       sizeof(DataNode) +
                                       (sizeof(DataNode*) * level));
        if (!base) {
            return nullptr;
        }
        new (base) Value(std::forward<ValueType>(value));

        auto node = (DataNode*)(base + sizeof(Value));
        node->level = level;
        memset(node->forward, 0, sizeof(DataNode*) * level);

        if (level > m_head.level) {
            for (uint32_t i = m_head.level; i < level; ++i) {
                update[i] = (DataNode*)(&m_head);
            }
            m_head.level = level;
        }

        for (uint32_t i = 0; i < level; ++i) {
            node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = node;
        }

        return node;
    }

    void DoDestroy() {
        DataNode* cur = m_head.forward[0];
        while (cur) {
            auto next = cur->forward[0];
            auto pvalue = GetValueFromNode(cur);
            pvalue->~Value();
            this->Free(pvalue);
            cur = next;
        }
    }

    uint32_t GenRandomLevel() const {
        uint32_t level = 1;
        while (level < MAX_LEVEL && xoshiro256ss_next(&m_rand) % 4 == 0) {
            ++level;
        }
        return level;
    }

    static Value* GetValueFromNode(const DataNode* node) {
        return (Value*)((char*)node - sizeof(Value));
    }

private:
    HeadNode m_head;
    Comparator m_cmp;
    GetKeyFromValue m_get_key;
    mutable Xoshiro256ss m_rand;

public:
    SkipList(SkipList&&) = default;
    SkipList& operator=(SkipList&&) = default;

private:
    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;
};

/* -------------------------------------------------------------------------- */

namespace internal {

template <typename Value>
struct SkipListReturnSelfFromValue final {
    const Value& operator()(const Value& value) const {
        return value;
    }
};

template <typename Key, typename Value>
struct SkipListReturnFirstOfPair final {
    const Key& operator()(const std::pair<Key, Value>& p) const {
        return p.first;
    }
};

template <typename Key>
struct GenericComparator final {
    uint32_t operator()(const Key& a, const Key& b) const {
        if (a == b) {
            return SKIPLIST_DIFF_EQ;
        }
        if (a < b) {
            return SKIPLIST_DIFF_LT;
        }
        return SKIPLIST_DIFF_GT;
    }
};

}

template <typename Value, typename Comparator = internal::GenericComparator<Value>,
          typename Allocator = GenericCpuAllocator>
using SkipListSet = SkipList<Value, Value, Comparator,
                             internal::SkipListReturnSelfFromValue<Value>,
                             Allocator>;

template <typename Key, typename Value, typename Comparator = internal::GenericComparator<Key>,
          typename Allocator = GenericCpuAllocator>
using SkipListMap = SkipList<Key, std::pair<Key, Value>, Comparator,
                             internal::SkipListReturnFirstOfPair<Key, Value>,
                             Allocator>;

}

#endif
