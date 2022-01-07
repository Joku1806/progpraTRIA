#pragma once

#include <packets/TRIA_GenericPacket.h>

class TRIA_RangeReport : public TRIA_GenericPacket {
public:
  static const size_t PACKED_SIZE = TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE + 2 * TRIA_Stamp::PACKED_SIZE;
  static const size_t FIELD_COUNT = 5;

  TRIA_RangeReport() {
    auto a = TRIA_Action(range_report);
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

  TRIA_RangeReport(TRIA_ID sid, TRIA_ID rid, TRIA_Stamp rx, TRIA_Stamp tx) {
    auto a = TRIA_Action(range_response);

    m_fields[0] = (TRIA_Field *)&a;
    m_fields[1] = (TRIA_Field *)&sid;
    m_fields[2] = (TRIA_Field *)&rid;
    m_fields[3] = (TRIA_Field *)&rx;
    m_fields[4] = (TRIA_Field *)&tx;
  };

  size_t field_count() override { return FIELD_COUNT; }
  void print() override;
  TRIA_Stamp get_rx_stamp() { return *(TRIA_Stamp *)m_fields[rx_stamp_position]; }
  TRIA_Stamp get_tx_stamp() { return *(TRIA_Stamp *)m_fields[tx_stamp_position]; }
};