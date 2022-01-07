#pragma once

#include <packets/TRIA_GenericPacket.h>

class TRIA_RangeReport : public TRIA_GenericPacket {
public:
  static const size_t PACKED_SIZE =
      TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE + 2 * TRIA_Stamp::PACKED_SIZE;
  static const size_t FIELD_COUNT = 5;

  TRIA_RangeReport()
    : m_action(TRIA_Action(range_report)), m_sender_id(TRIA_ID()), m_receiver_id(TRIA_ID()),
      m_rx_stamp(TRIA_Stamp()), m_tx_stamp(TRIA_Stamp()) {

    m_fields[0] = (TRIA_Field *)&m_action;
    m_fields[1] = (TRIA_Field *)&m_sender_id;
    m_fields[2] = (TRIA_Field *)&m_receiver_id;
    m_fields[3] = (TRIA_Field *)&m_rx_stamp;
    m_fields[4] = (TRIA_Field *)&m_tx_stamp;
  };

  TRIA_RangeReport(const TRIA_ID &sid, const TRIA_ID &rid, const TRIA_Stamp &rx,
                   const TRIA_Stamp &tx)
    : m_action(TRIA_Action(range_report)), m_sender_id(sid), m_receiver_id(rid), m_rx_stamp(rx),
      m_tx_stamp(tx) {

    m_fields[0] = (TRIA_Field *)&m_action;
    m_fields[1] = (TRIA_Field *)&m_sender_id;
    m_fields[2] = (TRIA_Field *)&m_receiver_id;
    m_fields[3] = (TRIA_Field *)&m_rx_stamp;
    m_fields[4] = (TRIA_Field *)&m_tx_stamp;

    Serial.printf("(RangeResponse@0x%p) Feldadressen direkt nach Initialisierung: \n", this);
    print_field_addresses();
    Serial.print("\n");
  };

  size_t field_count() override { return FIELD_COUNT; }
  void print() override;
  TRIA_Stamp get_rx_stamp() { return m_rx_stamp; }
  TRIA_Stamp get_tx_stamp() { return m_tx_stamp; }

private:
  TRIA_Action m_action;
  TRIA_ID m_sender_id;
  TRIA_ID m_receiver_id;
  TRIA_Stamp m_rx_stamp;
  TRIA_Stamp m_tx_stamp;
};