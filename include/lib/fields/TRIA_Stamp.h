#pragma once

#include <lib/fields/TRIA_Field.h>

class TRIA_Stamp : TRIA_Field {
public:
  static const size_t PACKED_SIZE = 5;

  TRIA_Stamp() : m_stamp(0) {}
  TRIA_Stamp(uint64_t stamp) : m_stamp(stamp) {}
  void set_value(uint64_t stamp);
  uint64_t value() { return m_stamp; }

  TRIA_Stamp operator-(TRIA_Stamp other);

  size_t pack_into(uint8_t *bytes) override;
  size_t packed_size() override;
  void initialise_from_buffer(uint8_t *buffer) override;

private:
  uint64_t m_stamp;
};