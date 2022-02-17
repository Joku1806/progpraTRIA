#pragma once

#include <fields/TRIA_Field.h>

class TRIA_Sequence : public TRIA_Field {
public:
  static const size_t PACKED_SIZE = sizeof(uint8_t);

  TRIA_Sequence() : m_sequence_no(0) {};
  TRIA_Sequence(uint8_t no) : m_sequence_no(no) {};

  size_t pack_into(uint8_t *bytes) override;
  size_t packed_size() override;
  void initialise_from_buffer(uint8_t *buffer) override;
  void print() override;

  void increment() { m_sequence_no++; }
  uint8_t value() { return m_sequence_no; }

private:
  uint8_t m_sequence_no;
};