#pragma once

#include <packets/TRIA_GenericPacket.h>

class TRIA_Measure : public TRIA_GenericPacket {
public:
  static const size_t PACKED_SIZE = TRIA_ID::PACKED_SIZE + TRIA_Stamp::PACKED_SIZE;
  static const size_t FIELD_COUNT = 2;

  TRIA_Measure() : m_tag(TRIA_ID()), m_timediff(TRIA_Stamp()) { overwrite_fields(); };

  TRIA_Measure(const TRIA_ID &tag, const TRIA_Stamp &timediff) : m_tag(tag), m_timediff(timediff) {
    overwrite_fields();
  };

  TRIA_Measure(const TRIA_Measure &other) : m_tag(other.m_tag), m_timediff(other.m_timediff) {
    overwrite_fields();
  }

  TRIA_Measure &operator=(const TRIA_Measure &other) {
    m_tag = other.m_tag;
    m_timediff = other.m_timediff;

    overwrite_fields();
    return *this;
  }

  ~TRIA_Measure() {}

private:
  TRIA_ID m_tag;
  TRIA_Stamp m_timediff;

  void overwrite_fields() override {
    m_fields[0] = (TRIA_Field *)&m_tag;
    m_fields[1] = (TRIA_Field *)&m_timediff;
  }

  size_t field_count() override { return FIELD_COUNT; }
};