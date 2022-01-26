#include <Arduino.h>
#include <packets/TRIA_RangeReport.h>

void TRIA_RangeReport::print() {
  TRIA_GenericPacket::print();
  Serial.print("\n");

  // für Timer Zeug siehe User Manual S. 21
  // c_air = 299702458
  // ToF_s = (RX - TX) * 15.65 picoseconds  / 1000000000000
  // distance = (ToF_s / 2) * c_air;
  TRIA_Stamp dt = get_rx_stamp() - get_tx_stamp();
  double distance = dt.value() * 0.00234517173385;

  Serial.print("Distanz von ");
  received_from().print();
  Serial.print(" zu ");
  sent_to().print();
  Serial.print(" ist ");
  Serial.print(distance);
  Serial.print("m.\n");
}

void TRIA_RangeReport::print_distance() {
  TRIA_Stamp dt = get_rx_stamp() - get_tx_stamp();
  double distance = dt.value() * 0.00234517173385;
  Serial.print(distance, 10);
  Serial.print("\n");
}