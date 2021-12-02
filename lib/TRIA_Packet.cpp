#include <lib/TRIA_Packet.h>
#include <Arduino.h>
#include <vector>

bool TRIA_Packet::is_addressed_to(TRIA_ID id) {
  return id.matches_mask(m_receiver_id);
}

bool TRIA_Packet::is_type(action a) {
  return m_action.value() == a;
}

size_t TRIA_Packet::packed_size() {
  switch (m_action.value()) {
    case range_request:
      return m_action.packed_size() + m_sender_id.packed_size() + m_receiver_id.packed_size();
    case range_response:
    case range_report:
      return m_action.packed_size() + m_sender_id.packed_size() + m_receiver_id.packed_size() +
             m_rx_stamp.packed_size() + m_tx_stamp.packed_size();
    default:
      // TODO: Fehlermeldung unknown packet type
      break;
  }
}

void TRIA_Packet::print() {
  switch (m_action.value()) {
    case range_request:
      Serial.printf("Range Request von Unit %u an Unit(s) %u\n", *m_sender_id.packed(), *m_receiver_id.packed());
    case range_report:
      size_t c_air = 299702458;
      TRIA_Stamp dt = m_rx_stamp - m_tx_stamp;
      float distance = dt.value() / 2.0 * c_air;
      Serial.printf("Distanz von Unit %u nach Unit %u: %fm\n", *m_sender_id.packed(), *m_receiver_id.packed(), distance);
  }
}

uint8_t* TRIA_Packet::packed() {
  std::vector<TRIA_Field*> fields;

  switch (m_action.value()) {
    case range_request:
      fields = {(TRIA_Field*)&m_sender_id, (TRIA_Field*)&m_receiver_id};
      break;
    case range_response:
    case range_report:
      fields = {(TRIA_Field*)&m_sender_id, (TRIA_Field*)&m_receiver_id, (TRIA_Field*)&m_rx_stamp, (TRIA_Field*)&m_tx_stamp};
      break;
    default:
      // TODO: Fehlermeldung unknown packet type
      break;
  }

  size_t offset = 0;
  for (auto field : fields) {
    uint8_t* bytes = field->packed();
    for (size_t i = 0; i < field->packed_size(); i++) {
      *(m_packed + offset) = bytes[i];
      offset++;
    }
  }

  // TODO: prüfen ob offset == packed_size()
  return m_packed;
}

void TRIA_Packet::initialise_from_buffer(uint8_t *buffer) {
  m_action.initialise_from_buffer(buffer);
  buffer += m_action.packed_size();
  std::vector<TRIA_Field*> rest;
  
  switch (m_action.value()) {
    case range_request:
      rest = {(TRIA_Field*)&m_sender_id, (TRIA_Field*)&m_receiver_id};
      break;
    case range_response:
    case range_report:
      rest = {(TRIA_Field*)&m_sender_id, (TRIA_Field*)&m_receiver_id, (TRIA_Field*)&m_rx_stamp, (TRIA_Field*)&m_tx_stamp};
      break;
    default:
      // TODO: Fehlermeldung unknown packet type
      break;
  }

  for (auto field : rest) {
    field->initialise_from_buffer(buffer);
    buffer += field->packed_size();
  }
}