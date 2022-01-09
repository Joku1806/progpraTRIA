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
  Serial.println("Habe Paket bekommen!");
  auto got_report = interface.handle_incoming_packet(cb_data->datalength, report);
  if (!got_report) {
    return;
  }

  report.print();
}

void tx_handler(const dwt_cb_data_t *cb_data) {
  Serial.println("Paket wurde gesendet!");
  (void)cb_data;
  interface.save_tx_stamp();
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_TX);
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
  interface = DW3000_Interface(id, tx_handler, recv_handler);
}

void loop() {
#ifdef SENDER
  interface.send_packet(&request);
  delay(1000);
  interface.receive_packet_mock(TRIA_RangeRequest::PACKED_SIZE + 2, report);
  delay(1000);
  interface.receive_packet_mock(TRIA_RangeResponse::PACKED_SIZE + 2, report);
  report.print();
  delay(1000);
#endif
}