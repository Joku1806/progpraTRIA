#include <Arduino.h>
#include <cstring>
#include <fields/TRIA_Stamp.h>
#include <inttypes.h>
#include <lib/assertions.h>

TRIA_Stamp TRIA_Stamp::operator-(TRIA_Stamp other) {
  VERIFY(m_stamp >= other.m_stamp);
  return TRIA_Stamp(m_stamp - other.m_stamp);
}

size_t TRIA_Stamp::pack_into(uint8_t *bytes) {
  VERIFY(m_stamp <= 0x000000FFFFFFFFFF);
  uint64_t stamp_nb = m_stamp;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  stamp_nb = __builtin_bswap64(stamp_nb);
#endif

  memcpy(bytes, &stamp_nb, PACKED_SIZE);
  return PACKED_SIZE;
}

size_t TRIA_Stamp::packed_size() { return PACKED_SIZE; }

void TRIA_Stamp::initialise_from_buffer(uint8_t *buffer) {
  uint64_t stamp_nb = 0;
  memcpy(&stamp_nb, buffer, PACKED_SIZE);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  stamp_nb = __builtin_bswap64(stamp_nb);
#endif

  m_stamp = stamp_nb;
  VERIFY(m_stamp <= 0x000000FFFFFFFFFF);
}

void TRIA_Stamp::initialise_from_buffer_no_bswap(uint8_t *buffer) {
  m_stamp = 0;
  memcpy(&m_stamp, buffer, PACKED_SIZE - 3);
  VERIFY(m_stamp <= 0x000000FFFFFFFFFF);
}

void TRIA_Stamp::print() {
  Serial.print("0x");
  Serial.print((uint32_t)(m_stamp >> 32), HEX);
  Serial.print((uint32_t)(m_stamp & 0xffffffff), HEX);
}