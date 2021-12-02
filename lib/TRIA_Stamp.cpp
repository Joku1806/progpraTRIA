#include <lib/TRIA_Stamp.h>

void TRIA_Stamp::set_value(uint64_t stamp) {
  m_stamp = stamp;
}

// FIXME: Sollte vielleicht neue Stamp zurückgeben
// und alte unangetastet lassen?
TRIA_Stamp TRIA_Stamp::operator-(TRIA_Stamp other) {
  m_stamp -= other.m_stamp;
  return *this;
}

uint8_t *TRIA_Stamp::packed() {
  return (uint8_t*)(&m_stamp);
}

size_t TRIA_Stamp::packed_size() {
  return STAMP_SIZE;
}

void TRIA_Stamp::initialise_from_buffer(uint8_t* buffer) {
  for (size_t i = 0; i < packed_size(); i++) {
    m_stamp |= buffer[i] << (i * 8);
  }
}