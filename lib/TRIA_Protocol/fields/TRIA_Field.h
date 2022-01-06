#pragma once

#include <cstddef>
#include <cstdint>

class TRIA_Field {
public:
  virtual ~TRIA_Field() {};
  virtual size_t pack_into(uint8_t *buffer) = 0;
  virtual size_t packed_size() = 0;
  virtual void initialise_from_buffer(uint8_t *buffer) = 0;
  virtual void print() = 0;
};