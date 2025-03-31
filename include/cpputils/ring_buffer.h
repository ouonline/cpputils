#ifndef __CPPUTILS_RING_BUFFER_H__
#define __CPPUTILS_RING_BUFFER_H__

#include <stdint.h>
#include <vector>

namespace cpputils {

template <typename T>
class RingBuffer final {
public:
    RingBuffer(uint32_t capacity) {
        m_tail = -1;
        m_head = m_size = 0;
        m_buffer.resize(capacity);
    }

    RingBuffer(std::vector<T>&& vec) {
        m_head = 0;
        m_tail = vec.size() - 1;
        m_size = vec.size();
        m_buffer = std::move(vec);
    }

    RingBuffer(RingBuffer&& rhs) {
        DoMove(std::move(rhs));
    }

    void operator=(RingBuffer&& rhs) {
        DoMove(std::move(rhs));
    }

    template <typename ItemType>
    void PushBack(ItemType&& item) {
        m_tail = (m_tail + 1) % m_buffer.size();
        m_buffer[m_tail] = std::forward<ItemType>(item);

        if (m_size == m_buffer.size()) {
            m_head = (m_head + 1) % m_buffer.size();
        } else {
            ++m_size;
        }
    }

    uint32_t size() const {
        return m_size;
    }

    uint32_t capacity() const {
        return m_buffer.size();
    }

    T& front() {
        return m_buffer[m_head];
    }

    const T& front() const {
        return m_buffer[m_head];
    }

    T& back() {
        return m_buffer[m_tail];
    }

    const T& back() const {
        return m_buffer[m_tail];
    }

    T& At(uint32_t idx) {
        auto pos = (m_head + idx) % m_buffer.size();
        return m_buffer[pos];
    }

    const T& At(uint32_t idx) const {
        auto pos = (m_head + idx) % m_buffer.size();
        return m_buffer[pos];
    }

    T& operator[](uint32_t idx) {
        return At(idx);
    }

    const T& operator[](uint32_t idx) const {
        return At(idx);
    }

private:
    void DoMove(RingBuffer&& rhs) {
        m_size = rhs.m_size;
        m_head = rhs.m_head;
        m_tail = rhs.m_tail;
        m_buffer = std::move(rhs.m_buffer);
        rhs.m_tail = -1;
        rhs.m_head = rhs.m_size = 0;
    }

private:
    uint32_t m_size;
    uint32_t m_head, m_tail;
    std::vector<T> m_buffer;
};

}

#endif
