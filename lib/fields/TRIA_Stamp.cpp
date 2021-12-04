#include <lib/fields/TRIA_Stamp.h>

void TRIA_Stamp::set_value(uint64_t stamp) {
  m_stamp = stamp;
}

// FIXME: Sollte vielleicht neue Stamp zurückgeben
// und alte unangetastet lassen?
TRIA_Stamp TRIA_Stamp::operator-(TRIA_Stamp other) {
  m_stamp -= other.m_stamp;
  return *this;
}

size_t TRIA_Stamp::pack_into(uint8_t *bytes) {
  uint8_t *from = (uint8_t *)&m_stamp;
  for (size_t i = 0; i < PACKED_SIZE; i++) {
    bytes[i] = from[i];
  }

  return PACKED_SIZE;
}

size_t TRIA_Stamp::packed_size() { return PACKED_SIZE; }

void TRIA_Stamp::initialise_from_buffer(uint8_t *buffer) {
  uint8_t *to = (uint8_t *)&m_stamp;
  for (size_t i = 0; i < PACKED_SIZE; i++) {
    to[i] = buffer[i];
  }
}