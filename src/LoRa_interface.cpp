
#include <Arduino.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <TRIA_helper.h>
#include <fields/TRIA_ID.h>
#include <lib/assertions.h>
#include <packets/TRIA_GenericPacket.h>
#include <packets/TRIA_RangeReport.h>
#include <packets/TRIA_RangeRequest.h>
#include <packets/TRIA_RangeResponse.h>
#include <platform/pin_mappings.h>
#include <src/DW3000_interface.h>
#include <src/USB_interface.h>

// Adafruit Feather M0 with RFM95
RH_RF95 rf95(8, 3);
DW3000_Interface DW_interface;
USB_Interface USB_interface;

TRIA_ID id;
uint8_t send_buffer[TRIA_GenericPacket::PACKED_SIZE];
uint8_t recv_buffer[TRIA_GenericPacket::PACKED_SIZE];

TRIA_RangeReport cached_request;
TRIA_RangeReport cached_response;
TRIA_RangeReport cached_report;

void build_id() {
  uint8_t type = 0;

#ifdef TRACKER
  type |= tracker;
#endif

#ifdef TRACKEE
  type |= trackee;
#endif

#ifdef COORDINATOR
  type |= coordinator;
#endif

#ifdef UNIT_ID
  id = TRIA_ID(static_cast<TRIA_dev_type>(type), UNIT_ID);
#else
  id = TRIA_ID(static_cast<TRIA_dev_type>(type), 1);
#endif
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

#ifdef DEBUG
  Serial.print("Habe Report bekommen: ");
  cached_report.print();
  Serial.print("\n");
#endif

  cached_report.print_distance();

  if (id.is_coordinator() && !USB_interface.schedule_full()) {
    USB_interface.schedule_report(cached_report);
  }
}

void setup() {
  // LoRa Chipselect auf HIGH schalten, damit er nicht während der SPI Kommunikation mit dem DW3000
  // stört.
  pinMode(LoRa_chipselect, OUTPUT);
  digitalWrite(LoRa_chipselect, HIGH);
  pinMode(SPI_chipselect, OUTPUT);
  SPI.begin();

#if defined(DEBUG) || defined(COORDINATOR)
  Serial.begin(9600);
  while (!Serial) {};
#endif

  build_id();
  DW_interface = DW3000_Interface(id, recv_handler);
  VERIFY(rf95.init());
}

void loop() {
  if (id.is_coordinator() &&
      (USB_interface.schedule_full() || USB_interface.schedule_likely_finished())) {
    USB_interface.send_scheduled_reports();
    USB_interface.schedule_reset();
  }

  if (rf95.available()) {
    uint8_t recv_length = 0;
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

    if (received->is_type(range_request) && received->received_from().is_coordinator()) {
      TRIA_RangeRequest repeated = TRIA_RangeRequest(id, TRIA_ID(tracker));
      lora_send_packet(repeated);
    }
  }

  if (id.is_coordinator() && USB_interface.measurement_requested()) {
    TRIA_RangeRequest request = TRIA_RangeRequest(id, TRIA_ID(trackee));
    lora_send_packet(request);
  }
}
