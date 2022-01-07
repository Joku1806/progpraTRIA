#include <Arduino.h>
#include <lib/assertions.h>
#include <packets/TRIA_GenericPacket.h>

bool TRIA_GenericPacket::is_addressed_to(TRIA_ID id) {
  return id.matches_mask(sent_to());
}

bool TRIA_GenericPacket::is_type(action a) {
  return ((TRIA_Action *)m_fields.at(action_position))->value() == a;
}

size_t TRIA_GenericPacket::pack_into(uint8_t *bytes) {
  size_t offset = 0;
  for (auto field : m_fields) {
    offset += field->pack_into(bytes + offset);
  }

  VERIFY(offset == packed_size());
  return PACKED_SIZE;
}

size_t TRIA_GenericPacket::packed_size() {
  size_t size = 0;
  for (auto field : m_fields) {
    size += field->packed_size();
  }

  return size;
}

void TRIA_GenericPacket::initialise_from_buffer(uint8_t *buffer) {
  for (auto field : m_fields) {
    field->initialise_from_buffer(buffer);
    buffer += field->packed_size();
  }
}

void TRIA_GenericPacket::print() {
  Serial.printf("(%u Felder) | ", m_fields.size());
  for (auto field : m_fields) {
    field->print();
    Serial.print(" | ");
  }
}