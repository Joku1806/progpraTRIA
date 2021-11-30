#include <lib/TRIA_Protocol.h>

bool TRIA_id::matches_mask(TRIA_id sent_mask) {
  if (sent_mask.m_type == 0 && sent_mask.m_id == 0) return true; // Nachricht an alle Empfänger
  if (sent_mask.m_type == this->m_type && sent_mask.m_id == 0) return true; // Nachricht an Empfänger mit bestimmten Typ
  return sent_mask.m_type == this->m_type && sent_mask.m_id == this->m_id; // Nachricht spezifisch an einen Empfänger
}