#include <RH_RF95.h>
#include <SPI.h>
#include <fields/TRIA_Field.h>
#include <lib/assertions.h>
#include <packets/TRIA_RangeReport.h>
#include <packets/TRIA_RangeRequest.h>
#include <platform/pin_mappings.h>
#include <src/DW3000_interface.h>
#include <string>

using namespace std;

#ifdef SENDER
TRIA_ID id = TRIA_ID(coordinator, 1);
#else
TRIA_ID id = TRIA_ID(coordinator, 2);
#endif
DW3000_Interface interface;
TRIA_RangeReport report;

static bool initiated = false;
RH_RF95 rf95(8, 3);
uint8_t m_protocol_buffer[TRIA_GenericPacket::PACKED_SIZE];

void receive_handler(const dwt_cb_data_t *cb_data) {
  auto got_report = interface.handle_incoming_packet(cb_data->datalength, report);
  if (!got_report) {
    return;
  }

  report.print();
}
// behandelt Befehl von usb schnittstelle
// true if new usb request available
bool usbHandler(uint8_t data[]) {
  /* Schnittstelle noch nicht klar deswegen noch nicht definiert */
}
void rangeReportHandler() {
  if (rf95.available()) {
    uint8_t buf[TRIA_GenericPacket::PACKED_SIZE;
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)) {
      TRIA_RangeReport a;
      a.initialise_from_buffer(buf);

      Serial.print("got a RangeReport: ");
      a.print();
      // muss an usb weitergeleitet werden -> noch zu implementieren!

    } else {
      Serial.println("no new RangeReports");
    }
  }
}

void coordinatorSendPacket(TRIA_GenericPacket &packet) {
  packet.pack_into(m_protocol_buffer);
  rf95.send(m_protocol_buffer, packet.packed_size());
}

// Methode um Lokalisierungsprozess zu starten -> an alle
void init() {

  Serial.println("Sending range request to all");
  // Send a range request to rf95_server

  TRIA_RangeRequest packet(id, TRIA_ID(tracker, 0));

  coordinatorSendPacket(packet);

  rf95.waitPacketSent();

  initiated = true;
}

// Methode für normalen tracking-Prozess
void sendRangeRequest() {
  TRIA_RangeRequest packet(id, TRIA_ID(trackee, 2));

  coordinatorSendPacket(packet);

  rf95.waitPacketSent();
}

void setup() {
  pinMode(LoRa_chipselect, OUTPUT);
  digitalWrite(LoRa_chipselect, HIGH);
  pinMode(SPI_chipselect, OUTPUT);
  SPI.begin();
  Serial.begin(9600);
  while (!Serial)
    ;
  interface = DW3000_Interface(id, receive_handler);

  if (!rf95.init())
    Serial.println("init failed");
}

void loop() {
#ifdef SENDER
  if (!initiated) {
    init();
  }
  // bool b = usbHandler(data); wenn usb anfrage
  bool b = true; // solange usbHandler noch nicht definiert
  if (b) {
    // startet Initialisierung bzw normalen Ranging Prozess
    sendRangeRequest();
  }
  delay(2000);
#else
  checken ob RangeReport oder USB anfrage da   -> noch zu implementieren!
  rangeReportHandler();
#endif
}
