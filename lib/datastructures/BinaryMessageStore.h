#pragma once

#include <packets/TRIA_GenericPacket.h>

struct BinaryMessage {
  uint64_t receive_time;
  uint8_t data[TRIA_GenericPacket::PACKED_SIZE];
  size_t data_length;
};

// TODO: custom Ringbuffer für mehrere Typen bereitstellen,
// könnte z.B. auch von USB_Interface benutzt werden.
class BinaryMessageStore {
public:
  static const size_t MAX_ITEMS = 32;

  BinaryMessageStore() : m_read_index(0), m_write_index(0) {};

  bool is_empty();
  bool is_full();

  void push_back(BinaryMessage &m);
  BinaryMessage pop_front();

private:
  BinaryMessage m_items[MAX_ITEMS];
  volatile size_t m_read_index;
  volatile size_t m_write_index;

  size_t next_index(size_t index);
};