#pragma once

#include <fields/TRIA_Action.h>
#include <fields/TRIA_Field.h>
#include <fields/TRIA_ID.h>
#include <fields/TRIA_Stamp.h>
#include <stddef.h>
#include <stdint.h>
#include <vector>

enum field_positions {
  action_position = 0,
  sender_id_position = 1,
  receiver_id_position = 2,
  rx_stamp_position = 3,
  tx_stamp_position = 4,
};

class TRIA_GenericPacket : public TRIA_Field {
public:
  // gehe von größtem Paket aus
  static const size_t PACKED_SIZE = TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE + 2 * TRIA_Stamp::PACKED_SIZE;

  bool is_type(action a);
  bool is_addressed_to(TRIA_ID id);
  TRIA_ID sent_to() { return *(TRIA_ID *)m_fields.at(receiver_id_position); }
  TRIA_ID received_from() { return *(TRIA_ID *)m_fields.at(sender_id_position); }

  void print();

  size_t pack_into(uint8_t *bytes) override;
  size_t packed_size() override;
  void initialise_from_buffer(uint8_t *buffer) override;

protected:
  std::vector<TRIA_Field *> m_fields;
};