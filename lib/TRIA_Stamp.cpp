#include <lib/TRIA_Stamp.h>

void TRIA_Stamp::set_value(uint64_t stamp) {
  m_stamp = stamp;
}

uint8_t *TRIA_Stamp::packed() {
  return (uint8_t*)(&m_stamp);
}

size_t TRIA_Stamp::packed_size() {
  return STAMP_SIZE;
}