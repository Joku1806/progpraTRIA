#include <lib/TRIA_Protocol/fields/TRIA_Stamp.h>
#include <lib/assertions.h>
#include <string>

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
  // FIXME: Ist __BIG_ENDIAN__ auf unserer Target Architektur definiert?
#if __BIG_ENDIAN__
  uint64_t stamp_nb = m_stamp;
#else
  uint64_t stamp_nb = __builtin_bswap64(m_stamp);
#endif
  memcpy(bytes, &stamp_nb, PACKED_SIZE);
  return PACKED_SIZE;
}

size_t TRIA_Stamp::packed_size() { return PACKED_SIZE; }

void TRIA_Stamp::initialise_from_buffer(uint8_t *buffer) {
  uint64_t stamp_nb = 0;
  memcpy(&stamp_nb, buffer, PACKED_SIZE);

#if __BIG_ENDIAN__
  m_stamp = stamp_nb;
#else
  m_stamp = __builtin_bswap64(stamp_nb);
#endif

  VERIFY(m_stamp <= 0x000000FFFFFFFFFF);
}