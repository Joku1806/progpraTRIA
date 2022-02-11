#pragma once

#include <packets/TRIA_GenericPacket.h>

class TRIA_RangeResponse : public TRIA_GenericPacket {
public:
  static const size_t PACKED_SIZE =
      TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE + 2 * TRIA_Stamp::PACKED_SIZE;
  static const size_t FIELD_COUNT = 5;

  TRIA_RangeResponse()
    : m_action(TRIA_Action(range_response)), m_sender_id(TRIA_ID()), m_receiver_id(TRIA_ID()),
      m_rx_stamp(TRIA_Stamp()), m_tx_stamp(TRIA_Stamp()) {
    overwrite_fields();
  };

  TRIA_RangeResponse(const TRIA_ID &sid, const TRIA_ID &rid, const TRIA_Stamp &rx)
    : m_action(TRIA_Action(range_response)), m_sender_id(sid), m_receiver_id(rid), m_rx_stamp(rx),
      m_tx_stamp(TRIA_Stamp()) {
    overwrite_fields();
  };

  TRIA_RangeResponse(const TRIA_RangeResponse &other)
    : m_action(other.m_action), m_sender_id(other.m_sender_id), m_receiver_id(other.m_receiver_id),
      m_rx_stamp(other.m_rx_stamp), m_tx_stamp(other.m_tx_stamp) {
    overwrite_fields();
  }

  TRIA_RangeResponse &operator=(const TRIA_RangeResponse &other) {
    m_action = other.m_action;
    m_sender_id = other.m_sender_id;
    m_receiver_id = other.m_receiver_id;
    m_rx_stamp = other.m_rx_stamp;
    m_tx_stamp = other.m_tx_stamp;

    overwrite_fields();
    return *this;
  }

  ~TRIA_RangeResponse() {}

  void set_tx_stamp(const TRIA_Stamp &tx) {
    m_tx_stamp = tx;
    m_fields[tx_stamp_position] = &m_tx_stamp;
  }

  TRIA_Stamp get_rx_stamp() { return m_rx_stamp; }
  TRIA_Stamp get_tx_stamp() { return m_tx_stamp; }

private:
  TRIA_Action m_action;
  TRIA_ID m_sender_id;
  TRIA_ID m_receiver_id;
  TRIA_Stamp m_rx_stamp;
  TRIA_Stamp m_tx_stamp;

  void overwrite_fields() override {
    // FIXME: die *_position sollten für jedes Paket einzeln definiert werden.
    m_fields[action_position] = (TRIA_Field *)&m_action;
    m_fields[sender_id_position] = (TRIA_Field *)&m_sender_id;
    m_fields[receiver_id_position] = (TRIA_Field *)&m_receiver_id;
    m_fields[rx_stamp_position] = (TRIA_Field *)&m_rx_stamp;
    m_fields[tx_stamp_position] = (TRIA_Field *)&m_tx_stamp;
  }

  size_t field_count() override { return FIELD_COUNT; }
};