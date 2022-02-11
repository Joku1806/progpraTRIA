#include <Arduino.h>
#include <lib/assertions.h>
#include <packets/TRIA_GenericPacket.h>

bool TRIA_GenericPacket::is_addressed_to(TRIA_ID &id) {
  TRIA_ID receiver = sent_to();
  return id.matches_mask(receiver);
}

bool TRIA_GenericPacket::is_type(action a) {
  return ((TRIA_Action *)m_fields[action_position])->value() == a;
}

size_t TRIA_GenericPacket::pack_into(uint8_t *bytes) {
  size_t offset = 0;
  for (size_t i = 0; i < field_count(); i++) {
    offset += m_fields[i]->pack_into(bytes + offset);
  }

  VERIFY(offset == packed_size());
  return packed_size();
}

size_t TRIA_GenericPacket::packed_size() {
  size_t size = 0;
  for (size_t i = 0; i < field_count(); i++) {
    size += m_fields[i]->packed_size();
  }

  return size;
}

void TRIA_GenericPacket::initialise_from_buffer(uint8_t *buffer) {
  for (size_t i = 0; i < field_count(); i++) {
    m_fields[i]->initialise_from_buffer(buffer);
    buffer += m_fields[i]->packed_size();
  }
}

void TRIA_GenericPacket::print() {
  const char *field_labels[FIELD_COUNT] = {"Typ", "Von", "An", "RX", "TX"};
  Serial.printf("(%u Felder) | ", field_count());
  for (size_t i = 0; i < field_count(); i++) {
    Serial.printf("%s: ", field_labels[i]);
    m_fields[i]->print();
    Serial.print(" | ");
  }
}