#include <Arduino.h>
#include <BinaryMessageStore.h>
#include <algorithm>
#include <lib/assertions.h>

size_t BinaryMessageStore::next_index(size_t index) { return (index + 1) % MAX_ITEMS; }

bool BinaryMessageStore::is_empty() { return m_read_index == m_write_index; }

bool BinaryMessageStore::is_full() { return m_read_index == next_index(m_write_index); }

void BinaryMessageStore::push_back(BinaryMessage &m) {
  if (is_full()) {
    m_read_index = next_index(m_read_index);
  }

  m_items[m_write_index].receive_time = m.receive_time;
  std::copy(m.data, m.data + m.data_length, m_items[m_write_index].data);
  m_items[m_write_index].data_length = m.data_length;

  m_write_index = next_index(m_write_index);
}

BinaryMessage BinaryMessageStore::pop_front() {
  VERIFY(!is_empty());

  size_t front_index = m_read_index;
  m_read_index = next_index(m_read_index);

  return m_items[front_index];
}