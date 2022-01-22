#include <Arduino.h>
#include <packets/TRIA_RangeReport.h>

void TRIA_RangeReport::print() {
  TRIA_GenericPacket::print();
  Serial.print("\n");

  double c_air = 299702458.0;
  TRIA_Stamp dt = get_rx_stamp() - get_tx_stamp();
  double tof_normalized = dt.value() / 125000000; // Einheit: 8ns => ns => s, könnte genauer sein, siehe User Manual S. 21
  double distance = tof_normalized / 2.0 * c_air;
  Serial.print("Distanz von ");
  received_from().print();
  Serial.print(" zu ");
  sent_to().print();
  Serial.print(" ist ");
  Serial.print(distance);
  Serial.print("m.\n");
}