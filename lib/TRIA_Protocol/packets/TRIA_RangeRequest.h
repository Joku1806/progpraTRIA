#pragma once

#include <packets/TRIA_GenericPacket.h>

class TRIA_RangeRequest : public TRIA_GenericPacket {
public:
  static const size_t PACKED_SIZE = TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE;
  static const size_t FIELD_COUNT = 3;

  TRIA_RangeRequest()
    : m_action(TRIA_Action(range_request)), m_sender_id(TRIA_ID()), m_receiver_id(TRIA_ID()) {

    m_fields[0] = (TRIA_Field *)&m_action;
    m_fields[1] = (TRIA_Field *)&m_sender_id;
    m_fields[2] = (TRIA_Field *)&m_receiver_id;
  };

  TRIA_RangeRequest(const TRIA_ID &sid, const TRIA_ID &rid)
    : m_action(TRIA_Action(range_request)), m_sender_id(sid), m_receiver_id(rid) {
    m_fields[0] = (TRIA_Field *)&m_action;
    m_fields[1] = (TRIA_Field *)&m_sender_id;
    m_fields[2] = (TRIA_Field *)&m_receiver_id;

    Serial.printf("(RangeRequest@0x%p) Feldadressen direkt nach Initialisierung: \n", this);
    print_field_addresses();
    Serial.print("\n");
  };

  size_t field_count() override { return FIELD_COUNT; }

private:
  TRIA_Action m_action;
  TRIA_ID m_sender_id;
  TRIA_ID m_receiver_id;
};