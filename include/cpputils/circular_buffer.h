#ifndef __CPPUTILS_CIRCULAR_BUFFER_H__
#define __CPPUTILS_CIRCULAR_BUFFER_H__

#include <stdint.h>
#include <vector>

namespace cpputils {

template <typename T>
class CircularBuffer final {
public:
    CircularBuffer(uint32_t max_size) {
        m_tail = -1;
        m_head = m_size = 0;
        m_buffer.resize(max_size);
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

    uint32_t Size() const {
        return m_size;
    }

    T& Front() {
        return m_buffer[m_head];
    }

    const T& Front() const {
        return m_buffer[m_head];
    }

    T& Back() {
        return m_buffer[m_tail];
    }

    const T& Back() const {
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

private:
    uint32_t m_size;
    uint32_t m_head, m_tail;
    std::vector<T> m_buffer;
};

}

#endif
