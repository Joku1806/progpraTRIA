
#include <Arduino.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <fields/TRIA_ID.h>
#include <packets/TRIA_GenericPacket.h>
#include <packets/TRIA_RangeRequest.h>
#include <packets/TRIA_RangeReport.h>
#include <packets/TRIA_RangeResponse.h>
#include <platform/pin_mappings.h>
#include <lib/assertions.h>
#include <src/DW3000_interface.h>

// Adafruit Feather M0 with RFM95
RH_RF95 rf95(8, 3);
DW3000_Interface DW_interface;

TRIA_ID id = TRIA_ID(trackee, 1);
uint8_t send_buffer[TRIA_GenericPacket::PACKED_SIZE];
uint8_t recv_buffer[TRIA_GenericPacket::PACKED_SIZE];

TRIA_RangeReport cached_request;
TRIA_RangeReport cached_response;
TRIA_RangeReport cached_report;

// FIXME: Sollte in eigene Datei, weil DW3000_Interface die Logik auch braucht
bool packet_ok(uint8_t* nw_bytes, uint8_t received_length, TRIA_ID& receiver_id) {
  if (received_length > TRIA_GenericPacket::PACKED_SIZE) {
    return false;
  }

  TRIA_Action a;
  a.initialise_from_buffer(nw_bytes);

  switch (a.value()) {
    case range_request:
      if (received_length != TRIA_RangeRequest::PACKED_SIZE) { return false; }
      break;
    case range_response:
      if (received_length != TRIA_RangeResponse::PACKED_SIZE) { return false; }
      break;
    case range_report:
      if (received_length != TRIA_RangeReport::PACKED_SIZE) { return false; }
      break;
    default: VERIFY_NOT_REACHED();
  }

  TRIA_ID receive_mask;
  receive_mask.initialise_from_buffer(nw_bytes + TRIA_Action::PACKED_SIZE +
                                      TRIA_ID::PACKED_SIZE);
  if (!receiver_id.matches_mask(receive_mask)) {
    return false;
  }

  return true;
}

void lora_send_packet(TRIA_GenericPacket &packet) {
  packet.pack_into(send_buffer);
  rf95.send(send_buffer, packet.packed_size());
  rf95.waitPacketSent();
}

void recv_handler(const dwt_cb_data_t *cb_data) {
  auto got_report = DW_interface.handle_incoming_packet(cb_data->datalength, cached_report);
  if (!got_report) {
    return;
  }

  // TODO: counter aktualisieren, um außerhalb der Interruptroutine
  // entscheiden zu können, ob empfangener Range Report an Data Team weitergeleitet werden soll. (Jonas)
  cached_report.print();
}

void setup() {
  // LoRa Chipselect auf HIGH schalten, damit er nicht während der SPI Kommunikation mit dem DW3000
  // stört.
  pinMode(LoRa_chipselect, OUTPUT);
  digitalWrite(LoRa_chipselect, HIGH);
  pinMode(SPI_chipselect, OUTPUT);
  SPI.begin();

  Serial.begin(9600);
  while (!Serial);
  
  DW_interface = DW3000_Interface(id, recv_handler);
  VERIFY(rf95.init());
}


void loop() {
  if (!rf95.available()) {
    return;
  }

  uint8_t recv_length = sizeof(recv_buffer);
  if (!rf95.recv(recv_buffer, &recv_length)) {
    return;
  }

  if (!packet_ok(recv_buffer, recv_length, id)) {
    return;
  }

  TRIA_Action action;
  action.initialise_from_buffer(recv_buffer);

  TRIA_GenericPacket *received;
  switch (action.value()) {
    case range_request: received = &cached_request; break;
    case range_response: received = &cached_response; break;
    case range_report: received = &cached_report; break;
    default: VERIFY_NOT_REACHED();
  }

  // TODO: wenn Range Request und von Coordinator geschickt, dann selbst weiterversenden (Janis)
  // TODO: wenn Range Report und man selbst Coordinator ist, dann über USB an Data Team schicken,
  // counter benutzen (siehe oben im recv_handler) (Greta/Simon)

  // TODO: wenn man selbst Coordinator ist, dann Annahme von Commands über USB und Ausführung (Greta/Simon)
}
