#include <Arduino.h>
#include <fields/TRIA_Action.h>

action TRIA_Action::value() { return m_action; };

size_t TRIA_Action::pack_into(uint8_t *bytes) {
  *bytes = (uint8_t)m_action;
  return PACKED_SIZE;
}

size_t TRIA_Action::packed_size() { return PACKED_SIZE; }

void TRIA_Action::initialise_from_buffer(uint8_t *buffer) { m_action = (action)*buffer; }

void TRIA_Action::print() {
  switch (m_action) {
    case ping: Serial.print("Ping"); break;
    case data_pong: Serial.print("Data Pong"); break;
    case measure_report: Serial.print("Measure Report"); break;
  }
}