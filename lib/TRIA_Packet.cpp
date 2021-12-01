#include <lib/TRIA_Packet.h>

bool TRIA_Packet::is_addressed_to(TRIA_ID id) {
  return id.matches_mask(m_receiver_id);
}

bool TRIA_Packet::is_range_request() {
  return m_action.value() == range_request;
}

size_t TRIA_Packet::packed_size() {
  return MAX_PACKET_SIZE; // fürs erste, sollte aber davon abhängig gemacht werden, welche action das Paket hat
}

uint8_t* TRIA_Packet::packed() {
  size_t offset = 0;
  TRIA_Field* fields[5] = {(TRIA_Field*)&m_action, (TRIA_Field*)&m_receiver_id, (TRIA_Field*)&m_sender_id, (TRIA_Field*)&m_rx_stamp, (TRIA_Field*)&m_tx_stamp};

  size_t field_index = 0;
  size_t byte_offset = 0;

  while (field_index < 5 && byte_offset < packed_size()) {
    uint8_t* bytes = fields[field_index]->packed();
    for (size_t i = 0; i < fields[field_index]->packed_size(); i++) {
      *(m_packed + offset) = bytes[i];
      offset++;
    }
  }

  return m_packed;
}