

#include <RH_RF95.h>
#include <SPI.h>
#include <lib/TRIA_Protocol/fields/TRIA_ID.h>
#include <lib/TRIA_Protocol/packets/TRIA_GenericPacket.h>
#include <lib/TRIA_Protocol/packets/TRIA_RangeRequest.h>

RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95
int8_t rangingRequestCounter = 0;

TRIA_ID id = TRIA_ID(trackee, 1);
uint8_t m_trackee_buffer[TRIA_GenericPacket::PACKED_SIZE];

void trackeeHandleRangeRequest(TRIA_GenericPacket a) {
  if (rangingRequestCounter == 0) {
    TRIA_RangeRequest a(id, TRIA_ID(tracker, 0));
    trackeeSendPacket(a);

    rf95.waitPacketSent();

    TRIA_RangeRequest p(id, TRIA_ID(trackee, 0));

    trackeeSendPacket(p);

    rf95.waitPacketSent();
  }
}
void trackeeHandleRangeReport(TRIA_GenericPacket a) {
}

void trackeeHandleRangeResponse(TRIA_GenericPacket a) {
}

void trackeeSendPacket(TRIA_GenericPacket &packet) {
  packet.pack_into(m_trackee_buffer);
  rf95.send(m_trackee_buffer, packet.packed_size());
}
void setup() {
  Serial.begin(9600);
  while (!Serial)
    ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");
}

void loop() {
  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)) {

      TRIA_GenericPacket a;
      a.initialise_from_buffer(buf);
      a.print();
      // checken ob was für trackee angekommen
      if (a.is_addressed_to(id)) {

        if (a.is_type(range_request)) {
          // initialisierung
          trackeeHandleRangeRequest(a);
          rangingRequestCounter++;
        } else if (a.is_type(range_report)) {
          trackeeHandleRangeReport(a);
        } else {
          trackeeHandleRangeResponse(a);
        }
      }
    } else {
      Serial.println("no new Packets");
    }
  }
}
