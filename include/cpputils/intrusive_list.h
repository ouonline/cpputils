#ifndef __CPPUTILS_INTRUSIVE_LIST_H__
#define __CPPUTILS_INTRUSIVE_LIST_H__

#include "intrusive_list_node.h"

namespace cpputils {

template <typename Container, typename Tag>
class IntrusiveList final {
public:
    void PushBack(IntrusiveListNode<Container, Tag>* node) {
        node->AttachBefore(&m_head);
    }

    void PushFront(IntrusiveListNode<Container, Tag>* node) {
        node->AttachAfter(&m_head);
    }

    bool IsEmpty() const {
        return (m_head.m_next == &m_head);
    }

    template <typename CallBack>
    void ForEach(CallBack&& cb) {
        for (auto cur = m_head.m_next; cur != &m_head; cur = cur->m_next) {
            cb(cur->container());
        }
    }

    template <typename CallBack>
    void ForEachSafe(CallBack&& cb) {
        for (auto cur = m_head.m_next, next = cur->m_next; cur != &m_head;
             cur = next, next = cur->m_next) {
            cb(cur->container());
        }
    }

    template <typename CallBack>
    void ForEachReverse(CallBack&& cb) {
        for (auto cur = m_head.m_prev; cur != &m_head; cur = cur->m_prev) {
            cb(cur->container());
        }
    }

    template <typename CallBack>
    void ForEachReverseSafe(CallBack&& cb) {
        for (auto cur = m_head.m_prev, prev = cur->m_prev; cur != &m_head;
             cur = prev, prev = cur->m_prev) {
            cb(cur->container());
        }
    }

private:
    IntrusiveListNode<Container, Tag> m_head;
};

}

#endif
