#ifndef __CPPUTILS_SKIPLIST_H__
#define __CPPUTILS_SKIPLIST_H__

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>

namespace outils {

template <typename Key, typename Value, typename LessComparator, typename GetKeyFromValue>
class SkipList final {
private:
    static constexpr uint32_t SKIPLIST_MAX_LEVEL = 8;
    static constexpr int SKIPLIST_RAND_MAX = (RAND_MAX >> 2);

private:
    struct DataNode final {
        uintptr_t level;
        DataNode* forward[0];
    } __attribute__((aligned(sizeof(uintptr_t))));

    struct HeadNode final {
        uintptr_t level;
        DataNode* forward[SKIPLIST_MAX_LEVEL];
    } __attribute__((aligned(sizeof(uintptr_t))));

public:
    class Iterator final {
    public:
        Iterator() : m_node(nullptr) {}
        Value* operator-> () {
            return GetValueFromNode(m_node);
        }
        const Value* operator-> () const {
            return GetValueFromNode(m_node);
        }
        Value& operator* () {
            return *GetValueFromNode(m_node);
        }
        const Value& operator* () const {
            return *GetValueFromNode(m_node);
        }
        bool operator== (const Iterator& it) const {
            return (m_node == it.m_node);
        }
        bool operator!= (const Iterator& it) const {
            return (m_node != it.m_node);
        }
        void operator++ () {
            m_node = m_node->forward[0];
        }

    private:
        friend class SkipList;
        Iterator(DataNode* node) : m_node(node) {}

    private:
        DataNode* m_node;
    };

public:
    SkipList() {
        memset(&m_head, 0, sizeof(HeadNode));
    }

    ~SkipList() {
        InnerDestroy();
    }

    std::pair<Iterator, bool> Insert(const Value& value) {
        const Key& key = m_get_key(value);
        DataNode* update[SKIPLIST_MAX_LEVEL];

        auto node = InnerLookupGreaterOrEqual(key, update);
        if (node) {
            auto pvalue = GetValueFromNode(node);
            if (!m_less(key, m_get_key(*pvalue))) {
                return std::pair<Iterator, bool>(Iterator(node), false);
            }
        }

        node = InnerInsert(value, update);
        return std::pair<Iterator, bool>(Iterator(node), (node != nullptr));
    }

    bool Remove(const Key& key, Value* value = nullptr) {
        return InnerRemove(key, value, [this, &key] (const Key& greater_or_equal_key) -> bool {
            return (!m_less(key, greater_or_equal_key));
        });
    }

    bool RemoveGreaterOrEqual(const Key& key, Value* value = nullptr) {
        return InnerRemove(key, value, [] (const Key&) -> bool {
            return true;
        });
    }

    void Clear() {
        InnerDestroy();
        memset(&m_head, 0, sizeof(HeadNode));
    }

    Iterator Lookup(const Key& key) const {
        auto node = InnerLookupGreaterOrEqual(key);
        if (node) {
            auto pvalue = GetValueFromNode(node);
            if (!m_less(key, m_get_key(*pvalue))) {
                return Iterator(node);
            }
        }
        return Iterator();
    }

    Iterator LookupGreaterOrEqual(const Key& key) const {
        auto node = InnerLookupGreaterOrEqual(key);
        return Iterator(node);
    }

    Iterator LookupPrev(const Key& key) const {
        auto prev = InnerLookupPrev(key);
        if (prev == (DataNode*)(&m_head)) {
            return Iterator();
        }
        return Iterator(prev);
    }

    bool IsEmpty() const {
        return (m_head.level == 0);
    }

    Iterator GetBeginIterator() const {
        return Iterator(m_head.forward[0]);
    }

    const Iterator& GetEndIterator() const {
        return m_end_iterator;
    }

private:
    DataNode* InnerLookupPrev(const Key& key, DataNode** update = nullptr) const {
        auto prev = (DataNode*)(&m_head);
        for (uint32_t l = prev->level; l > 0; --l) {
            const uint32_t level = l - 1;
            auto node = prev->forward[level];
            while (node) {
                auto pvalue = GetValueFromNode(node);
                if (!m_less(m_get_key(*pvalue), key)) {
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

    DataNode* InnerLookupGreaterOrEqual(const Key& key, DataNode** update = nullptr) const {
        auto prev = InnerLookupPrev(key, update);
        return prev->forward[0];
    }

    bool InnerRemove(const Key& key, Value* value,
                     const std::function<bool (const Key& greater_or_equal_key)>& should_remove) {
        DataNode* update[SKIPLIST_MAX_LEVEL];
        auto node = InnerLookupGreaterOrEqual(key, update);
        if (!node) {
            return false;
        }

        auto pvalue = GetValueFromNode(node);
        if (!should_remove(m_get_key(*pvalue))) {
            return false;
        }

        for (uint32_t level = 0; level < m_head.level; ++level) {
            if (update[level]->forward[level] != node) {
                break;
            }
            update[level]->forward[level] = node->forward[level];
        }

        if (value) {
            *value = *pvalue;
        }

        pvalue->~Value();
        free(node);

        while (m_head.level > 0 && !m_head.forward[m_head.level - 1]) {
            --m_head.level;
        }

        return true;
    }

    DataNode* InnerInsert(const Value& value, DataNode* update[]) {
        const uint32_t level = GenRandomLevel();

        auto node = (DataNode*)aligned_alloc(sizeof(uintptr_t),
                                             sizeof(DataNode) +
                                             (sizeof(DataNode*) * level) +
                                             Align(sizeof(Value), sizeof(uintptr_t)));
        if (!node) {
            return nullptr;
        }

        node->level = level;
        memset(node->forward, 0, sizeof(DataNode*) * level);
        new (GetValueFromNode(node)) Value(value);

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

    void InnerDestroy() {
        DataNode* cur = m_head.forward[0];
        while (cur) {
            auto next = cur->forward[0];
            auto pvalue = GetValueFromNode(cur);
            pvalue->~Value();
            free(cur);
            cur = next;
        }
    }

    uint32_t GenRandomLevel() const {
        uint32_t level = 1;
        while (level < SKIPLIST_MAX_LEVEL && rand() < SKIPLIST_RAND_MAX) {
            ++level;
        }
        return level;
    }

    uint64_t Align(uint64_t n, uint32_t alignment) const {
        return ((n + alignment - 1) & (~(alignment - 1)));
    }

    static Value* GetValueFromNode(const DataNode* node) {
        return (Value*)((char*)node + sizeof(DataNode) + sizeof(DataNode*) * node->level);
    }

private:
    HeadNode m_head;
    LessComparator m_less;
    GetKeyFromValue m_get_key;
    Iterator m_end_iterator;

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
    const Value& operator () (const Value& value) const {
        return value;
    }
};

template <typename Key, typename Value>
struct SkipListReturnFirstOfPair final {
    const Key& operator () (const std::pair<Key, Value>& p) const {
        return p.first;
    }
};

}

template <typename Value, typename LessComparator = std::less<Value>>
using SkipListSet = SkipList<Value, Value, LessComparator,
                             internal::SkipListReturnSelfFromValue<Value>>;

template <typename Key, typename Value, typename LessComparator = std::less<Key>>
using SkipListMap = SkipList<Key, std::pair<Key, Value>, LessComparator,
                             internal::SkipListReturnFirstOfPair<Key, Value>>;

}

#endif
