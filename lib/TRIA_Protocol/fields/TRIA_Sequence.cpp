#include <Arduino.h>
#include <fields/TRIA_Sequence.h>

size_t TRIA_Sequence::pack_into(uint8_t *bytes) {
  *bytes = m_sequence_no;
  return PACKED_SIZE;
}

size_t TRIA_Sequence::packed_size() { return PACKED_SIZE; }

void TRIA_Sequence::initialise_from_buffer(uint8_t *buffer) { m_sequence_no = *buffer; }

void TRIA_Sequence::print() { Serial.print(m_sequence_no); }
