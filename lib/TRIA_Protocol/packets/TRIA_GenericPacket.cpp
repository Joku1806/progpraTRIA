#include <Arduino.h>
#include <lib/TRIA_Protocol/packets/TRIA_GenericPacket.h>
#include <lib/assertions.h>

bool TRIA_GenericPacket::is_addressed_to(TRIA_ID id) {
  return id.matches_mask(sent_to());
}

bool TRIA_GenericPacket::is_type(action a) {
  return ((TRIA_Action *)m_fields.at(action_position))->value() == a;
}

void TRIA_GenericPacket::print() {
  Serial.printf("Generic TRIA Packet.\n");
}

size_t TRIA_GenericPacket::pack_into(uint8_t *bytes) {
  size_t offset = 0;
  for (auto field : m_fields) {
    offset += field->pack_into(bytes + offset);
  }

  VERIFY(offset == PACKED_SIZE);
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