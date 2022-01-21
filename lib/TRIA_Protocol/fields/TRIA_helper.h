#pragma once

#include <fields/TRIA_Field.h>

class TRIA_helper : public TRIA_Field {
public:
  static const size_t PACKED_SIZE = sizeof(uint8_t);

  TRIA_helper : m_helper() {};
  TRIA_helper(helper h) : m_helper(h) {};

  size_t pack_into(uint8_t *buffer) override;
  size_t packed_size() override;
  void initialise_from_buffer(uint8_t *buffer) override;
  void print() override;

};
