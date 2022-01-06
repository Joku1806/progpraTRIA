#include <src/DW3000_interface.h>
#include <src/Tracker.h>
#include <platform/pin_mappings.h>

#ifdef SENDER
TRIA_ID id = TRIA_ID(tracker, 1);
#else
TRIA_ID id = TRIA_ID(tracker, 2);
#endif
DW3000_Interface interface;
TRIA_RangeReport report;

void receive_handler(const dwt_cb_data_t *cb_data) {
  auto got_report = interface.handle_incoming_packet(cb_data->datalength, report);
  if (!got_report) {
    return;
  }

  report.print();
}

void setup() {
  // LoRa Chipselect auf HIGH schalten, damit er nicht während der SPI Kommunikation mit dem DW3000 stört.
  pinMode(LoRa_chipselect, OUTPUT);
  digitalWrite(LoRa_chipselect, HIGH);
  pinMode(SPI_chipselect, OUTPUT);
  SPI.begin();
  Serial.begin(9600);
  while (!Serial);
  interface = DW3000_Interface(id, receive_handler);
}

void loop() {
#ifdef SENDER
  auto t = TRIA_RangeRequest(id, TRIA_ID(tracker, 0));
  interface.send_packet(t);
  delay(2000);
#endif
}