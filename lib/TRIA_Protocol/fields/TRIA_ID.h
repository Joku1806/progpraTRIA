#pragma once

#include <fields/TRIA_Field.h>

enum TRIA_dev_type {
  tracker = 1 << 7,
  coordinator = 1 << 6,
  tracker_coordinator = tracker | coordinator,
  trackee = 1 << 5,
};

class TRIA_ID : public TRIA_Field {
public:
  static const size_t PACKED_SIZE = sizeof(uint8_t);

  TRIA_ID() : m_id(0) {};
  TRIA_ID(uint8_t id) : m_id(id) {};
  TRIA_ID(TRIA_dev_type type, uint8_t id) : m_id(type | (id & 0x1f)) {};
  TRIA_dev_type type();
  uint8_t id();
  bool matches_mask(TRIA_ID& mask);

  size_t pack_into(uint8_t *bytes) override;
  size_t packed_size() override;
  void initialise_from_buffer(uint8_t *buffer) override;
  void print() override;

private:
  uint8_t m_id;
};