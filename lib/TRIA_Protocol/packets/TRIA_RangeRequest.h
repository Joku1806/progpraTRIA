#pragma once

#include <packets/TRIA_GenericPacket.h>

class TRIA_RangeRequest : public TRIA_GenericPacket {
public:
  static const size_t PACKED_SIZE = TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE;
  static const size_t FIELD_COUNT = 3;

  TRIA_RangeRequest()
    : m_action(TRIA_Action(range_request)), m_sender_id(TRIA_ID()), m_receiver_id(TRIA_ID()) {
    overwrite_fields();
  };

  TRIA_RangeRequest(const TRIA_ID &sid, const TRIA_ID &rid)
    : m_action(TRIA_Action(range_request)), m_sender_id(sid), m_receiver_id(rid) {
    overwrite_fields();
  };

  TRIA_RangeRequest(const TRIA_RangeRequest &other)
    : m_action(other.m_action), m_sender_id(other.m_sender_id), m_receiver_id(other.m_receiver_id) {
    overwrite_fields();
  }

  TRIA_RangeRequest &operator=(const TRIA_RangeRequest &other) {
    m_action = other.m_action;
    m_sender_id = other.m_sender_id;
    m_receiver_id = other.m_receiver_id;

    overwrite_fields();
    return *this;
  }

  ~TRIA_RangeRequest() {}

private:
  TRIA_Action m_action;
  TRIA_ID m_sender_id;
  TRIA_ID m_receiver_id;

  const char *m_field_labels[FIELD_COUNT] = {"Typ", "Von", "An"};
  const char **field_labels() override { return m_field_labels; };

  void overwrite_fields() override {
    m_fields[action_position] = (TRIA_Field *)&m_action;
    m_fields[sender_id_position] = (TRIA_Field *)&m_sender_id;
    m_fields[receiver_id_position] = (TRIA_Field *)&m_receiver_id;
  }

  size_t field_count() override { return FIELD_COUNT; }
};