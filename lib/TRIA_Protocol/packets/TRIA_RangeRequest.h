#pragma once

#include <packets/TRIA_GenericPacket.h>

class TRIA_RangeRequest : public TRIA_GenericPacket {
public:
  static const size_t PACKED_SIZE = TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE;
  static const size_t FIELD_COUNT = 3;

  TRIA_RangeRequest() {
    auto a = TRIA_Action(range_request);
    auto sid = TRIA_ID();
    auto rid = TRIA_ID();

    m_fields[0] = (TRIA_Field *)&a;
    m_fields[1] = (TRIA_Field *)&sid;
    m_fields[2] = (TRIA_Field *)&rid;
  };

  TRIA_RangeRequest(TRIA_ID sid, TRIA_ID rid) {
    auto a = TRIA_Action(range_request);

    m_fields[0] = (TRIA_Field *)&a;
    m_fields[1] = (TRIA_Field *)&sid;
    m_fields[2] = (TRIA_Field *)&rid;

    Serial.printf("Adresse m_fields (request) = %p\n", &m_fields);
  };

  size_t field_count() override { return FIELD_COUNT; }
};