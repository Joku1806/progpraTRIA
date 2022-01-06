#pragma once

#include <packets/TRIA_GenericPacket.h>

class TRIA_RangeResponse : public TRIA_GenericPacket {
public:
  static const size_t PACKED_SIZE = TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE + TRIA_Stamp::PACKED_SIZE;

  TRIA_RangeResponse() {
    auto a = TRIA_Action(range_response);
    auto sid = TRIA_ID();
    auto rid = TRIA_ID();
    auto rx = TRIA_Stamp();
    auto tx = TRIA_Stamp();

    m_fields = {
        (TRIA_Field *)&a,
        (TRIA_Field *)&sid,
        (TRIA_Field *)&rid,
        (TRIA_Field *)&rx,
        (TRIA_Field *)&tx,
    };
  };

  TRIA_RangeResponse(TRIA_ID sid, TRIA_ID rid, TRIA_Stamp rx) {
    auto a = TRIA_Action(range_response);

    m_fields = {
        (TRIA_Field *)&a,
        (TRIA_Field *)&sid,
        (TRIA_Field *)&rid,
        (TRIA_Field *)&rx,
        nullptr, // FIXME: Ist das in Ordnung?
    };
  };

  TRIA_Stamp set_tx_stamp(TRIA_Stamp &tx) { m_fields.at(tx_stamp_position) = &tx; }
  TRIA_Stamp get_rx_stamp() { return *(TRIA_Stamp *)m_fields.at(rx_stamp_position); }
  TRIA_Stamp get_tx_stamp() { return *(TRIA_Stamp *)m_fields.at(tx_stamp_position); }
};