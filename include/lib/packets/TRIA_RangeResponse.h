#pragma once

#include <lib/packets/TRIA_GenericPacket.h>

class TRIA_RangeResponse : public TRIA_GenericPacket {
public:
  static const size_t PACKED_SIZE = TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE + TRIA_Stamp::PACKED_SIZE;

  TRIA_RangeResponse() {
    m_fields = {
        (TRIA_Field *)&TRIA_Action(),
        (TRIA_Field *)&TRIA_ID(),
        (TRIA_Field *)&TRIA_ID(),
        (TRIA_Field *)&TRIA_Stamp(),
    };
  };

  TRIA_RangeResponse(TRIA_ID sid, TRIA_ID rid, TRIA_Stamp rx) {
    m_fields = {
        (TRIA_Field *)&TRIA_Action(range_response),
        (TRIA_Field *)&sid,
        (TRIA_Field *)&rid,
        (TRIA_Field *)&rx,
    };
  };

  TRIA_Stamp get_rx_stamp() { return *(TRIA_Stamp *)m_fields.at(rx_stamp_position); }
  TRIA_Stamp get_tx_stamp() { return *(TRIA_Stamp *)m_fields.at(tx_stamp_position); }
};