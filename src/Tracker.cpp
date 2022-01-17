#include <decadriver/deca_regs.h>
#include <platform/pin_mappings.h>
#include <src/DW3000_interface.h>
#include <src/Tracker.h>

#ifdef SENDER
TRIA_ID id = TRIA_ID(tracker_coordinator, 25);
#else
TRIA_ID id = TRIA_ID(tracker, 2);
#endif
DW3000_Interface interface;

TRIA_RangeRequest request = TRIA_RangeRequest(id, TRIA_ID(tracker, 0));
TRIA_RangeReport report;

// FIXME: Sollten wahrscheinlich Teil von DW3000_Interface sein
void recv_handler(const dwt_cb_data_t *cb_data) {
  auto got_report = interface.handle_incoming_packet(cb_data->datalength, report);
  if (!got_report) {
    return;
  }

  report.print();
}

void setup() {
  delay(5000);
  // LoRa Chipselect auf HIGH schalten, damit er nicht während der SPI Kommunikation mit dem DW3000
  // stört.
  pinMode(LoRa_chipselect, OUTPUT);
  digitalWrite(LoRa_chipselect, HIGH);
  pinMode(SPI_chipselect, OUTPUT);
  SPI.begin();
  Serial.begin(9600);
  while (!Serial) {}
  interface = DW3000_Interface(id, recv_handler);
}

void loop() {
#ifdef SENDER
  interface.send_packet(&request); // DW 1
  delay(1000);
  interface.receive_packet_mock(TRIA_RangeRequest::PACKED_SIZE + 2, report); // DW 2
  delay(1000);
  interface.receive_packet_mock(TRIA_RangeResponse::PACKED_SIZE + 2, report); // DW 1
  report.print();
  delay(1000);
#endif
}