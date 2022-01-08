#include <Arduino.h>
#include <cstring>
#include <fields/TRIA_Stamp.h>
#include <inttypes.h>
#include <lib/assertions.h>

void TRIA_Stamp::set_value(uint64_t stamp) {
  VERIFY(stamp <= 0x000000FFFFFFFFFF);
  m_stamp = stamp;
}

TRIA_Stamp TRIA_Stamp::operator-(TRIA_Stamp other) {
  // FIXME: bin mir nicht sicher, ob wir das wirklich
  // prüfen sollten, unsigned overflow ist schon mal
  // nicht UB und es kann sein, dass wir so eine
  // Berechnung machen müssen.
  VERIFY(m_stamp >= other.m_stamp);
  return TRIA_Stamp(m_stamp - other.m_stamp);
}

size_t TRIA_Stamp::pack_into(uint8_t *bytes) {
  VERIFY(m_stamp <= 0x000000FFFFFFFFFF);
  uint64_t stamp_nb = m_stamp;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  stamp_nb = __builtin_bswap64(stamp_nb);
#endif

  // Wichtig! So lassen, shift-Operatoren sind unabhängig
  // von Byteorder. Sollte aber vielleicht in den #if check
  // mit rein, nochmal darüber nachdenken
  stamp_nb >>= (sizeof(stamp_nb) - PACKED_SIZE) * 8;

  memcpy(bytes, &stamp_nb, PACKED_SIZE);
  return PACKED_SIZE;
}

size_t TRIA_Stamp::packed_size() { return PACKED_SIZE; }

void TRIA_Stamp::initialise_from_buffer(uint8_t *buffer) {
  Serial.println("Initialising stamp from buffer.");
  uint64_t stamp_nb = 0;
  memcpy(&stamp_nb, buffer, PACKED_SIZE);
  // FIXME: Muss das erst nach dem Byteswap gemacht werden?
  stamp_nb >>= (sizeof(stamp_nb) - PACKED_SIZE) * 8;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  m_stamp = __builtin_bswap64(stamp_nb);
#endif

  VERIFY(m_stamp <= 0x000000FFFFFFFFFF);
}

void TRIA_Stamp::print() {
  Serial.print("0x");
  Serial.print((uint32_t)(m_stamp >> 32), HEX);
  Serial.print((uint32_t)(m_stamp & 0xffffffff), HEX);
}