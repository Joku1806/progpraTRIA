#include <Arduino.h>
#include <packets/TRIA_RangeReport.h>

void TRIA_RangeReport::print() {
  Serial.print("| ");
  for (auto field : m_fields) {
    field->print();
    Serial.print(" | ");
  }

  double c_air = 299702458.0;
  TRIA_Stamp dt = get_rx_stamp() - get_tx_stamp();
  Serial.print(dt.value() / 2.0);
  double distance = dt.value() / 2.0 * c_air;
  Serial.print("Distanz von ");
  received_from().print();
  Serial.print(" nach ");
  sent_to().print();
  Serial.print(" ist ");
  Serial.print(distance);
  Serial.print("m.\n");
}