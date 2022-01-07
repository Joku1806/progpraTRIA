#pragma once

#include <packets/TRIA_GenericPacket.h>

class TRIA_RangeResponse : public TRIA_GenericPacket {
public:
  static const size_t PACKED_SIZE = TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE + 2 * TRIA_Stamp::PACKED_SIZE;
  static const size_t FIELD_COUNT = 5;

  TRIA_RangeResponse() {
    auto a = TRIA_Action(range_response);
    auto sid = TRIA_ID();
    auto rid = TRIA_ID();
    auto rx = TRIA_Stamp();
    auto tx = TRIA_Stamp();

    m_fields[0] = (TRIA_Field *)&a;
    m_fields[1] = (TRIA_Field *)&sid;
    m_fields[2] = (TRIA_Field *)&rid;
    m_fields[3] = (TRIA_Field *)&rx;
    m_fields[4] = (TRIA_Field *)&tx;
  };

  TRIA_RangeResponse(TRIA_ID sid, TRIA_ID rid, TRIA_Stamp rx) {
    auto a = TRIA_Action(range_response);
    auto unset_tx = TRIA_Stamp();

    m_fields[0] = (TRIA_Field *)&a;
    m_fields[1] = (TRIA_Field *)&sid;
    m_fields[2] = (TRIA_Field *)&rid;
    m_fields[3] = (TRIA_Field *)&rx;
    m_fields[4] = (TRIA_Field *)&unset_tx;

    Serial.printf("Adresse m_fields (response) = %p\n", &m_fields);
  };

  size_t field_count() override { return FIELD_COUNT; }

  void set_tx_stamp(TRIA_Stamp tx) { m_fields[tx_stamp_position] = &tx; }
  TRIA_Stamp get_rx_stamp() { return *(TRIA_Stamp *)m_fields[rx_stamp_position]; }
  TRIA_Stamp get_tx_stamp() { return *(TRIA_Stamp *)m_fields[tx_stamp_position]; }
};