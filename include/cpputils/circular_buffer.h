#ifndef __CPPUTILS_CIRCULAR_BUFFER_H__
#define __CPPUTILS_CIRCULAR_BUFFER_H__

#include <stdint.h>
#include <vector>

namespace cpputils {

template <typename T>
class CircularBuffer final {
public:
    CircularBuffer(uint32_t max_size) {
        m_head = m_tail = m_size = 0;
        m_buffer.resize(max_size);
    }

    uint32_t Size() const {
        return m_size;
    }

    bool IsFull() const {
        return (m_size == m_buffer.size());
    }

    void PushBack(const T& item) {
        m_buffer[m_tail] = item;
        AfterPushBack();
    }

    void PushBack(T&& item) {
        m_buffer[m_tail] = std::move(item);
        AfterPushBack();
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
    void AfterPushBack() {
        m_tail = (m_tail + 1) % m_buffer.size();

        if (IsFull()) {
            m_head = (m_head + 1) % m_buffer.size();
        } else {
            ++m_size;
        }
    }

private:
    uint32_t m_size;
    uint32_t m_head, m_tail;
    std::vector<T> m_buffer;
};

}

#endif
