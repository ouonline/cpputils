#ifndef __CPPUTILS_SKIPLIST_H__
#define __CPPUTILS_SKIPLIST_H__

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include "utils/utils.h"
using namespace std;

namespace outils {

template <typename Key, typename Value, typename Comparator, typename GetKeyFromValue>
class SkipList final {
private:
    static constexpr uint32_t SKIPLIST_MAX_LEVEL = 8;
    static constexpr int SKIPLIST_RAND_MAX = (RAND_MAX >> 2);

private:
    struct Node final {
        uint32_t level;
        Node* forward[0];
    };

    struct DataNode final {
        Value value;
        Node node;
    };

    struct HeadNode final {
        uint32_t level;
        Node* forward[SKIPLIST_MAX_LEVEL];
    };

public:
    SkipList() {
        memset(&m_head, 0, sizeof(HeadNode));
    }

    ~SkipList() {
        Node* cur = m_head.forward[0];
        Node* next = nullptr;
        while (cur) {
            next = cur->forward[0];
            auto data_node = container_of(cur, DataNode, node);
            data_node->~DataNode();
            free(data_node);
            cur = next;
        }
    }

    std::pair<Value*, bool> Insert(const Value& value, bool allow_multi_values = false) {
        Node* update[SKIPLIST_MAX_LEVEL];
        const Key& key = m_get_key(value);
        auto node = InternalLookupGreaterOrEqual(key, [&update] (uint32_t level, const Node* x) {
            update[level] = const_cast<Node*>(x);
        });
        if (!allow_multi_values) {
            if (node) {
                auto data_node = container_of(node, DataNode, node);
                if (!m_cmp(key, m_get_key(data_node->value))) {
                    return std::pair<Value*, bool>(&data_node->value, false);
                }
            }
        }

        node = InsertNode(value, update);
        if (node) {
            auto data_node = container_of(node, DataNode, node);
            return std::pair<Value*, bool>(&data_node->value, true);
        }

        return std::pair<Value*, bool>(nullptr, false);
    }

    bool Remove(const Key& key, Value* value = nullptr) {
        return InternalRemove(key, value, [this, &key] (const Key& node_key) -> bool {
            return !m_cmp(key, node_key);
        });
    }

    bool RemoveGreaterOrEqual(const Key& key, Value* value = nullptr) {
        return InternalRemove(key, value, [] (const Key&) -> bool {
            return true;
        });
    }

    void ForEach(const std::function<bool (const Value&)>& f) const {
        auto node = m_head.forward[0];
        while (node) {
            auto data_node = container_of(node, DataNode, node);
            if (!f(data_node->value)) {
                return;
            }
            node = node->forward[0];
        }
    }

    Value* Lookup(const Key& key) const {
        auto node = InternalLookupGreaterOrEqual(key, [] (uint32_t, const Node*) {});
        if (node) {
            auto data_node = container_of(node, DataNode, node);
            if (!m_cmp(key, m_get_key(data_node->value))) {
                return &data_node->value;
            }
        }
        return nullptr;
    }

    Value* LookupGreaterOrEqual(const Key& key) const {
        auto node = InternalLookupGreaterOrEqual(key, [] (uint32_t, const Node*) {});
        if (node) {
            auto data_node = container_of(node, DataNode, node);
            return &data_node->value;
        }
        return nullptr;
    }

private:
    Node* InternalLookupGreaterOrEqual(const Key& key,
                                       const std::function<void (uint32_t, const Node*)>& f) const {
        auto prev = (Node*)(&m_head);
        for (uint32_t l = prev->level; l > 0; --l) {
            uint32_t level = l - 1;
            auto node = prev->forward[level];
            while (node) {
                auto data_node = container_of(node, DataNode, node);
                if (!m_cmp(m_get_key(data_node->value), key)) {
                    break;
                }
                prev = node;
                node = node->forward[level];
            }

            f(level, prev);
        }

        return prev->forward[0];
    }

    bool InternalRemove(const Key& key, Value* value,
                        const std::function<bool (const Key& node_key)>& should_remove) {
        Node* update[SKIPLIST_MAX_LEVEL];
        auto node = InternalLookupGreaterOrEqual(key, [&] (uint32_t level, const Node* x) {
            update[level] = const_cast<Node*>(x);
        });

        if (node) {
            auto data_node = container_of(node, DataNode, node);
            if (should_remove(m_get_key(data_node->value))) {
                for (uint32_t level = 0; level < m_head.level; ++level) {
                    if (update[level]->forward[level] != node) {
                        break;
                    }
                    update[level]->forward[level] = node->forward[level];
                }

                if (value) {
                    *value = data_node->value;
                }

                data_node->~DataNode();
                free(data_node);

                while (m_head.level > 0 && !m_head.forward[m_head.level]) {
                    --m_head.level;
                }

                return true;
            }
        }

        return false;
    }

    Node* InsertNode(const Value& value, Node* update[]) {
        uint32_t level = GenRandomLevel();

        auto data_node = (DataNode*)malloc(sizeof(DataNode) + sizeof(Node*) * level);
        if (!data_node) {
            return nullptr;
        }

        new (data_node) DataNode();
        data_node->value = value;
        data_node->node.level = level;
        memset(data_node->node.forward, 0, sizeof(Node*) * level);

        if (level > m_head.level) {
            for (uint32_t i = m_head.level; i < level; ++i) {
                update[i] = (Node*)(&m_head);
            }
            m_head.level = level;
        }

        auto node = &data_node->node;
        for (uint32_t i = 0; i < level; ++i) {
            node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = node;
        }

        return node;
    }

    int GenRandomLevel() {
        uint32_t level = 1;
        while (level < SKIPLIST_MAX_LEVEL && rand() < SKIPLIST_RAND_MAX) {
            ++level;
        }
        return level;
    }

private:
    HeadNode m_head;
    Comparator m_cmp;
    GetKeyFromValue m_get_key;

public:
    SkipList(SkipList&&) = default;
    SkipList& operator=(SkipList&&) = default;

private:
    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;
};

/* -------------------------------------------------------------------------- */

namespace {

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

template <typename Value, typename Comparator = std::less<Value>>
using SkipListSet = SkipList<Value, Value, Comparator, SkipListReturnSelfFromValue<Value>>;

template <typename Key, typename Value, typename Comparator = std::less<Key>>
using SkipListMap = SkipList<Key,
                             std::pair<Key, Value>,
                             Comparator,
                             SkipListReturnFirstOfPair<Key, Value>>;

}

#endif
