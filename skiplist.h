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
    SkipList() {
        memset(&m_head, 0, sizeof(HeadNode));
    }

    ~SkipList() {
        InnerDestroy();
    }

    std::pair<Value*, bool> Insert(const Value& value) {
        DataNode* update[SKIPLIST_MAX_LEVEL];
        const Key& key = m_get_key(value);
        auto node = InnerLookupGreaterOrEqual(key, update);
        if (node) {
            auto pvalue = GetValueFromNode(node);
            if (!m_less(key, m_get_key(*pvalue))) {
                return std::pair<Value*, bool>(pvalue, false);
            }
        }

        node = InnerInsert(value, update);
        if (node) {
            return std::pair<Value*, bool>(GetValueFromNode(node), true);
        }

        return std::pair<Value*, bool>(nullptr, false);
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

    void ForEach(const std::function<bool (const Value&)>& f) const {
        auto node = m_head.forward[0];
        while (node) {
            auto pvalue = GetValueFromNode(node);
            if (!f(*pvalue)) {
                return;
            }
            node = node->forward[0];
        }
    }

    Value* Lookup(const Key& key) const {
        auto node = InnerLookupGreaterOrEqual(key);
        if (node) {
            auto pvalue = GetValueFromNode(node);
            if (!m_less(key, m_get_key(*pvalue))) {
                return pvalue;
            }
        }
        return nullptr;
    }

    Value* LookupGreaterOrEqual(const Key& key) const {
        auto node = InnerLookupGreaterOrEqual(key);
        if (node) {
            return GetValueFromNode(node);
        }
        return nullptr;
    }

    Value* LookupPrev(const Key& key) const {
        auto prev = InnerLookupPrev(key);
        if (prev == (DataNode*)(&m_head)) {
            return nullptr;
        }
        return GetValueFromNode(prev);
    }

    bool IsEmpty() const {
        return (m_head.level == 0);
    }

private:
    DataNode* InnerLookupPrev(const Key& key, DataNode** update = nullptr) const {
        auto prev = (DataNode*)(&m_head);
        for (uint32_t l = prev->level; l > 0; --l) {
            uint32_t level = l - 1;
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
        uint32_t level = GenRandomLevel();

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
        DataNode* next = nullptr;
        while (cur) {
            next = cur->forward[0];
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

    Value* GetValueFromNode(const DataNode* node) const {
        return (Value*)((char*)node + sizeof(DataNode) + sizeof(DataNode*) * node->level);
    }

    uint64_t Align(uint64_t n, uint32_t alignment) const {
        return ((n + alignment - 1) & (~(alignment - 1)));
    }

private:
    HeadNode m_head;
    LessComparator m_less;
    GetKeyFromValue m_get_key;

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
