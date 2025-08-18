#ifndef __CPPUTILS_RING_BUFFER_H__
#define __CPPUTILS_RING_BUFFER_H__

#include <stdint.h>
#include <vector>

namespace cpputils {

template <typename T>
class RingBuffer final {
private:
    void Init(uint32_t max_size) {
        m_tail = -1;
        m_head = m_size = 0;
        m_buffer.resize(max_size);
    }

public:
    RingBuffer(uint32_t max_size) {
        Init(max_size);
    }

    RingBuffer(std::vector<T>&& vec) {
        m_head = 0;
        m_tail = vec.size() - 1;
        m_size = vec.size();
        m_buffer = std::move(vec);
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

    bool IsEmpty() const {
        return (m_size == 0);
    }

    void Clear() {
        auto sz = m_buffer.size();
        m_buffer.clear();
        Init(sz);
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
    uint32_t m_size;
    uint32_t m_head, m_tail;
    std::vector<T> m_buffer;
};

}

#endif
