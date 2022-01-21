
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
#include <TRIA_helper.h>

// Adafruit Feather M0 with RFM95
RH_RF95 rf95(8, 3);
DW3000_Interface DW_interface;

TRIA_ID id = TRIA_ID(trackee, 1);
uint8_t send_buffer[TRIA_GenericPacket::PACKED_SIZE];
uint8_t recv_buffer[TRIA_GenericPacket::PACKED_SIZE];

TRIA_RangeReport cached_request;
TRIA_RangeReport cached_response;
TRIA_RangeReport cached_report;

volatile unsigned measure_counter = 0;
volatile bool measure_received = false;

void lora_send_packet(TRIA_GenericPacket &packet) {
  packet.pack_into(send_buffer);
  rf95.send(send_buffer, packet.packed_size());
  rf95.waitPacketSent();
}

void recv_handler(const dwt_cb_data_t *cb_data) {
  Serial.println("Habe Interrupt bekommen.");
  auto got_report = DW_interface.handle_incoming_packet(cb_data->datalength, cached_report);
  if (!got_report) {
    return;
  }

  cached_report.print();
  measure_counter++;
  measure_received = true;
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
  
#ifdef SENDER
  Serial.println("Sender");
#else
  Serial.println("Receiver");
#endif

  DW_interface = DW3000_Interface(id, recv_handler);
  VERIFY(rf95.init());
}


void loop() {
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

    // TODO: wenn Range Request und von Coordinator geschickt, dann selbst weiterversenden (Janis)
    // measure_counter muss hier wieder zurückgesetzt werden.
    
    // TODO: wenn Range Report und man selbst Coordinator ist, dann über USB an Data Team schicken (Greta/Simon)
    // FIXME: Können wir nicht einzeln rüberschicken, weil wir vorher wissen müssen, ob überhaupt 3 Messungen
    // ankommen, wir sollten also bei jedem Check die Differenz von der aktuellen Zeit und der Ankunftszeit des
    // letzten Pakets berechnen und damit entscheiden, ob wir alle bisherigen Pakete rübersenden sollen. 
    if (measure_received && measure_counter <= 3) {
      measure_received = false;
    }
  }

  // TODO: wenn man selbst Coordinator ist, dann Annahme von Commands über USB und Ausführung (Greta/Simon)
  #ifdef SENDER
  TRIA_RangeRequest request = TRIA_RangeRequest(id, TRIA_ID(0));
  DW_interface.send_packet(&request);
  delay(500);
  #endif
}
