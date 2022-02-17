#pragma once

#include <fields/TRIA_Sequence.h>
#include <packets/TRIA_GenericPacket.h>

class TRIA_Ping : public TRIA_GenericPacket {
public:
  static const size_t PACKED_SIZE =
      TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE + TRIA_Sequence::PACKED_SIZE;
  static const size_t FIELD_COUNT = 4;

  TRIA_Ping()
    : m_action(TRIA_Action(ping)), m_sender_id(TRIA_ID()), m_receiver_id(TRIA_ID()),
      m_sequence(TRIA_Sequence()) {
    overwrite_fields();
  };

  TRIA_Ping(const TRIA_ID &sid, const TRIA_ID &rid)
    : m_action(TRIA_Action(ping)), m_sender_id(sid), m_receiver_id(rid),
      m_sequence(TRIA_Sequence()) {
    overwrite_fields();
  };

  TRIA_Ping(const TRIA_Ping &other)
    : m_action(other.m_action), m_sender_id(other.m_sender_id), m_receiver_id(other.m_receiver_id),
      m_sequence(other.m_sequence) {
    overwrite_fields();
  }

  TRIA_Ping &operator=(const TRIA_Ping &other) {
    m_action = other.m_action;
    m_sender_id = other.m_sender_id;
    m_receiver_id = other.m_receiver_id;
    m_sequence = other.m_sequence;

    overwrite_fields();
    return *this;
  }

  ~TRIA_Ping() {}

  void increment_sequence() { m_sequence.increment(); }
  bool is_first() { return m_sequence.value() == 0; }

private:
  TRIA_Action m_action;
  TRIA_ID m_sender_id;
  TRIA_ID m_receiver_id;
  TRIA_Sequence m_sequence;

  const char *m_field_labels[FIELD_COUNT] = {"Typ", "Von", "An", "Sequence"};
  const char **field_labels() override { return m_field_labels; };

  void overwrite_fields() override {
    m_fields[action_position] = (TRIA_Field *)&m_action;
    m_fields[sender_id_position] = (TRIA_Field *)&m_sender_id;
    m_fields[receiver_id_position] = (TRIA_Field *)&m_receiver_id;
    m_fields[3] = (TRIA_Field *)&m_sequence;
  }

  size_t field_count() override { return FIELD_COUNT; }
};