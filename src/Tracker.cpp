#include <platform/pin_mappings.h>
#include <src/DW3000_interface.h>
#include <src/Tracker.h>

#ifdef SENDER
TRIA_ID id = TRIA_ID(tracker, 1);
#else
TRIA_ID id = TRIA_ID(tracker, 2);
#endif
DW3000_Interface interface;

TRIA_RangeRequest request = TRIA_RangeRequest(id, TRIA_ID(tracker_coordinator, 0));
TRIA_RangeResponse response =
    TRIA_RangeResponse(TRIA_ID(tracker_coordinator, 41), id, TRIA_Stamp(0x7d61226244));
TRIA_RangeReport report;

void receive_handler(const dwt_cb_data_t *cb_data) {
  Serial.println("Habe Paket bekommen!");
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
  interface = DW3000_Interface(id, receive_handler);

  Serial.printf("(RangeRequest@%p) ", &request);
  request.print_field_addresses();
  Serial.print("\n");

  Serial.printf("(RangeResponse@%p) ", &response);
  response.print_field_addresses();
  Serial.print("\n");
}

void loop() {
#ifdef SENDER
  interface.send_packet(&request);
  delay(1000);
  interface.send_packet(&response);
  delay(1000);
#endif
}