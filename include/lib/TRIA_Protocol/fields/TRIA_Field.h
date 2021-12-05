#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

class TRIA_Field {
public:
  TRIA_Field() {};
  virtual size_t pack_into(uint8_t *buffer) = 0;
  virtual size_t packed_size() = 0;
  virtual void initialise_from_buffer(uint8_t *buffer) = 0;
};