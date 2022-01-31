
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

RH_RF95 rf95(8, 3);
DW3000_Interface DW_interface;
USB_Interface USB_interface;

TRIA_ID id;
uint8_t recv_buffer[TRIA_GenericPacket::PACKED_SIZE];
uint8_t send_buffer[TRIA_GenericPacket::PACKED_SIZE];

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
#ifdef DEBUG
  Serial.print("Paket gesendet (LoRa): ");
  packet.print();
  Serial.print("\n");
#endif

  packet.pack_into(send_buffer);
  VERIFY(rf95.send(send_buffer, packet.packed_size()));
}

void recv_handler(const dwt_cb_data_t *cb_data) {
  DW_interface.store_received_message(cb_data);
  dwt_writefastCMD(CMD_RX);
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
  DW_interface = DW3000_Interface(recv_handler);
  VERIFY(rf95.init());
}

void loop() {
  while (DW_interface.unprocessed_messages_pending()) {
    BinaryMessage m = DW_interface.get_first_unprocessed_message();

    TRIA_GenericPacket *received = nullptr;
    if (!deserialise_packet(m.data, m.data_length, id, &received)) {
      continue;
    }
    VERIFY(received != nullptr);

#ifdef DEBUG
    Serial.println("Paket empfangen (DW): ");
    received->print();
    Serial.print("\n");
#endif

    if (received->is_type(range_request)) {
      auto response = TRIA_RangeResponse(id, received->received_from(), TRIA_Stamp(m.receive_time));
      DW_interface.send_range_response(response);
    } else if (received->is_type(range_response)) {
      TRIA_Stamp message_rx = TRIA_Stamp(m.receive_time);
      TRIA_Stamp original_tx = DW_interface.get_tx_stamp();
      TRIA_Stamp measured_rx = static_cast<TRIA_RangeResponse *>(received)->get_rx_stamp();
      TRIA_Stamp measured_tx = static_cast<TRIA_RangeResponse *>(received)->get_tx_stamp();
      // FIXME: vielleicht sollten wir die Ungenauigkeitschecks schon hier machen, anstatt auf
      // der Data Team Seite
      message_rx = message_rx - (measured_tx - measured_rx);

      auto report = TRIA_RangeReport(received->received_from(), id, message_rx, original_tx);
      lora_send_packet(report);
    }
  }

  if (id.is_coordinator() &&
      (USB_interface.schedule_full() || USB_interface.schedule_likely_finished())) {
    USB_interface.send_scheduled_reports();
    USB_interface.schedule_reset();
  }

  if (rf95.available()) {
    uint8_t recv_length = sizeof(recv_buffer);
    if (!rf95.recv(recv_buffer, &recv_length)) {
      return;
    }

    TRIA_GenericPacket *received = nullptr;
    if (!deserialise_packet(recv_buffer, recv_length, id, &received)) {
      return;
    }
    VERIFY(received != nullptr);

#ifdef DEBUG
    Serial.print("Paket empfangen (LoRa): ");
    received->print();
    Serial.print("\n");
#endif

    if (received->is_type(range_request) && received->received_from().is_coordinator()) {
      TRIA_RangeRequest repeated = TRIA_RangeRequest(id, TRIA_ID(tracker));
      DW_interface.send_range_request(repeated);
    }

    if (received->is_type(range_report) && id.is_coordinator() && !USB_interface.schedule_full()) {
      // FIXME: Irgendwie ohne cast hinkriegen
      USB_interface.schedule_report(*static_cast<TRIA_RangeReport *>(received));
    }
  }

  if (id.is_coordinator() && USB_interface.measurement_requested()) {
    TRIA_RangeRequest request = TRIA_RangeRequest(id, TRIA_ID(trackee));
    lora_send_packet(request);
  }
}
