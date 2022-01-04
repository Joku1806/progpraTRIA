#include <fields/TRIA_ID.h>

TRIA_dev_type TRIA_ID::type() { return (TRIA_dev_type)(m_id & 0xe0); }
uint8_t TRIA_ID::id() { return m_id & 0x1f; }

// FIXME: besseren Namen finden
bool TRIA_ID::matches_mask(TRIA_ID mask) {
  // Nachricht an alle Empfänger
  if (mask.type() == 0 && mask.id() == 0) {
    return true;
  }

  // Nachricht an Empfänger mit bestimmten Typ
  // ein Tracker | Coordinator darf eine Nachricht für Tracker annehmen,
  // aber nicht anders herum, d.h. mask muss ein subset der eigenen id sein
  bool mask_exceeds_id = false;
  for (size_t i = 0; i < 3; i++) {
    // FIXME: ist operator precedence hier richtig?
    if (mask.type() & 1 << (5 + i) && !(type() & 1 << (5 + i))) {
      mask_exceeds_id = true;
      break;
    }
  }

  if (!mask_exceeds_id && mask.id() == 0) {
    return true;
  }

  // Nachricht spezifisch an einen Empfänger
  return mask.type() == type() && mask.id() == id();
}

size_t TRIA_ID::pack_into(uint8_t *bytes) {
  *bytes = m_id;
  return PACKED_SIZE;
}

size_t TRIA_ID::packed_size() { return PACKED_SIZE; }

void TRIA_ID::initialise_from_buffer(uint8_t *buffer) { m_id = *buffer; }