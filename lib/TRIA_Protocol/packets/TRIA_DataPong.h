#pragma once

#include <packets/TRIA_GenericPacket.h>

class TRIA_DataPong : public TRIA_GenericPacket {
public:
  static const size_t PACKED_SIZE =
      TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE + 2 * TRIA_Stamp::PACKED_SIZE;
  static const size_t FIELD_COUNT = 5;

  TRIA_DataPong()
    : m_action(TRIA_Action(data_pong)), m_sender_id(TRIA_ID()), m_receiver_id(TRIA_ID()),
      m_timediff_D(TRIA_Stamp()), m_timediff_R(TRIA_Stamp()) {
    overwrite_fields();
  };

  TRIA_DataPong(const TRIA_ID &sid, const TRIA_ID &rid)
    : m_action(TRIA_Action(data_pong)), m_sender_id(sid), m_receiver_id(rid),
      m_timediff_D(TRIA_Stamp()), m_timediff_R(TRIA_Stamp()) {
    overwrite_fields();
  };

  TRIA_DataPong(const TRIA_DataPong &other)
    : m_action(other.m_action), m_sender_id(other.m_sender_id), m_receiver_id(other.m_receiver_id),
      m_timediff_D(other.m_timediff_D), m_timediff_R(other.m_timediff_R) {
    overwrite_fields();
  }

  TRIA_DataPong &operator=(const TRIA_DataPong &other) {
    m_action = other.m_action;
    m_sender_id = other.m_sender_id;
    m_receiver_id = other.m_receiver_id;
    m_timediff_D = other.m_timediff_D;
    m_timediff_R = other.m_timediff_R;

    overwrite_fields();
    return *this;
  }

  ~TRIA_DataPong() {}

  void set_timediff_D(const TRIA_Stamp &D) {
    m_timediff_D = D;
    m_fields[timediff_D_position] = &m_timediff_D;
  }

  void set_timediff_R(const TRIA_Stamp &R) {
    m_timediff_R = R;
    m_fields[timediff_R_position] = &m_timediff_R;
  }

  TRIA_Stamp get_timediff_D() { return m_timediff_D; }
  TRIA_Stamp get_timediff_R() { return m_timediff_R; }

private:
  TRIA_Action m_action;
  TRIA_ID m_sender_id;
  TRIA_ID m_receiver_id;
  TRIA_Stamp m_timediff_D;
  TRIA_Stamp m_timediff_R;

  const char *m_field_labels[FIELD_COUNT] = {"Typ", "Von", "An", "TimeDiff (D)", "TimeDiff (R)"};
  const char **field_labels() override { return m_field_labels; };

  void overwrite_fields() override {
    // FIXME: die *_position sollten für jedes Paket einzeln definiert werden.
    m_fields[action_position] = (TRIA_Field *)&m_action;
    m_fields[sender_id_position] = (TRIA_Field *)&m_sender_id;
    m_fields[receiver_id_position] = (TRIA_Field *)&m_receiver_id;
    m_fields[timediff_D_position] = (TRIA_Field *)&m_timediff_D;
    m_fields[timediff_R_position] = (TRIA_Field *)&m_timediff_R;
  }

  size_t field_count() override { return FIELD_COUNT; }
};