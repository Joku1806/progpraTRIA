#pragma once

#include <cstddef>
#include <lib/assertions.h>

template <typename T, size_t N> class Ringbuffer {
public:
  Ringbuffer() {};

  bool is_empty() { return m_read_index == m_write_index; }

  bool is_full() { return m_read_index == next_index(m_write_index); }

  size_t size() const {
    return m_write_index >= m_read_index ? m_write_index - m_read_index
                                         : (N + 1) - m_read_index + m_write_index;
  }

  T at(size_t index) const {
    VERIFY(index < size());
    return m_items[(m_read_index + index) % (N + 1)];
  }

  T *at_ptr(size_t index) {
    VERIFY(index < size());
    return m_items + ((m_read_index + index) % (N + 1));
  }

  void push_back(T &item) {
    if (is_full()) {
      m_read_index = next_index(m_read_index);
    }

    m_items[m_write_index] = item;
    m_write_index = next_index(m_write_index);
  }

  T pop_front() {
    T front = at(0);
    m_read_index = next_index(m_read_index);

    return front;
  }

  void clear() { m_read_index = m_write_index; }

private:
  T m_items[N + 1];
  volatile size_t m_read_index {0};
  volatile size_t m_write_index {0};

  size_t next_index(size_t index) { return (index + 1) % (N + 1); }
};