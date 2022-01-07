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
    Serial.printf("Adresse von a: %p\n", &a);

    m_fields[0] = (TRIA_Field *)&a;
    m_fields[1] = (TRIA_Field *)&sid;
    m_fields[2] = (TRIA_Field *)&rid;

    Serial.print("(RangeRequest) Feldadressen direkt nach Initialisierung: \n");
    print_field_addresses();
    Serial.print("\n");
  };

  size_t field_count() override { return FIELD_COUNT; }
};