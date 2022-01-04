#pragma once

#include <packets/TRIA_GenericPacket.h>

class TRIA_RangeRequest : public TRIA_GenericPacket {
public:
  static const size_t PACKED_SIZE = TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE;

  TRIA_RangeRequest() {
    auto a = TRIA_Action(range_request);
    auto sid = TRIA_ID();
    auto rid = TRIA_ID();

    m_fields = {
        (TRIA_Field *)&a,
        (TRIA_Field *)&sid,
        (TRIA_Field *)&rid,
    };
  };

  TRIA_RangeRequest(const TRIA_ID &sid, const TRIA_ID &rid) {
    auto a = TRIA_Action(range_request);

    m_fields = {
        (TRIA_Field *)&a,
        (TRIA_Field *)&sid,
        (TRIA_Field *)&rid,
    };
  };

  void print();
  TRIA_Stamp get_rx_stamp() { return *(TRIA_Stamp *)m_fields.at(rx_stamp_position); }
};