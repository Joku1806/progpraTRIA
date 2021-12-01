#include <lib/TRIA_ID.h>

TRIA_dev_type TRIA_ID::type() { return (TRIA_dev_type)((m_id & 0xe0) >> 5); }
uint8_t TRIA_ID::id() { return m_id & 0x1f; }

bool TRIA_ID::matches_mask(TRIA_ID mask) {
  if (mask.type() == 0 && mask.id() == 0) return true; // Nachricht an alle Empfänger
  if (mask.type() == type() && mask.id() == 0) return true; // Nachricht an Empfänger mit bestimmten Typ
  return mask.type() == type() && mask.id() == id(); // Nachricht spezifisch an einen Empfänger
}

uint8_t* TRIA_ID::packed() { return &m_id; }
size_t TRIA_ID::packed_size() { return sizeof(m_id); }