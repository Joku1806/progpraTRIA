#pragma once

#include <lib/packets/TRIA_GenericPacket.h>

class TRIA_RangeRequest : public TRIA_GenericPacket {
public:
  static const size_t PACKED_SIZE = TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE;

  TRIA_RangeRequest() {
    m_fields = {
        (TRIA_Field *)&TRIA_Action(),
        (TRIA_Field *)&TRIA_ID(),
        (TRIA_Field *)&TRIA_ID(),
    };
  };

  TRIA_RangeRequest(const TRIA_ID &sid, const TRIA_ID &rid) {
    m_fields = {
        (TRIA_Field *)&TRIA_Action(range_request),
        (TRIA_Field *)&sid,
        (TRIA_Field *)&rid,
    };
  };

  void print();
  TRIA_Stamp get_rx_stamp() { return *(TRIA_Stamp *)m_fields.at(rx_stamp_position); }
};