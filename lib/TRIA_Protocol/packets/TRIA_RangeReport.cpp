#include <Arduino.h>
#include <packets/TRIA_RangeReport.h>

void TRIA_RangeReport::print() {
  Serial.print("| ");
  for (auto field : m_fields) {
    field->print();
    Serial.print(" | ");
  }

  size_t c_air = 299702458;
  TRIA_Stamp dt = get_rx_stamp() - get_tx_stamp();
  float distance = dt.value() / 2.0 * c_air;
  Serial.printf("Distanz von Unit %u nach Unit %u: %fm\n", received_from(), sent_to(), distance);
}