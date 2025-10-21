#ifndef __CPPUTILS_INTRUSIVE_LIST_NODE_H__
#define __CPPUTILS_INTRUSIVE_LIST_NODE_H__

namespace cpputils {

template <typename Container, typename Tag>
class IntrusiveList;

template <typename Container, typename Tag>
class IntrusiveListNode {
private:
    void Reset() {
        m_prev = this;
        m_next = this;
    }

public:
    IntrusiveListNode() {
        Reset();
    }

    virtual ~IntrusiveListNode() {}

    Container* container() {
        return static_cast<Container*>(this);
    }

    void AttachBefore(IntrusiveListNode* next) {
        m_next = next;
        m_prev = next->m_prev;
        next->m_prev = this;
        m_prev->m_next = this;
    }

    void AttachAfter(IntrusiveListNode* prev) {
        m_prev = prev;
        m_next = prev->m_next;
        prev->m_next = this;
        m_next->m_prev = this;
    }

    void DetachWithoutReset() {
        m_prev->m_next = m_next;
        m_next->m_prev = m_prev;
    }

    void Detach() {
        DetachWithoutReset();
        Reset();
    }

private:
    friend class IntrusiveList<Container, Tag>;
    IntrusiveListNode* m_prev;
    IntrusiveListNode* m_next;
};

}

#endif
