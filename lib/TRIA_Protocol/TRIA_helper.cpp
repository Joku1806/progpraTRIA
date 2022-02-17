#include <Arduino.h>
#include <TRIA_helper.h>
#include <lib/assertions.h>
#include <packets/TRIA_DataPong.h>
#include <packets/TRIA_MeasureReport.h>
#include <packets/TRIA_Ping.h>

// FIXME: Sehr hässliche Art, das Problem der Paket Initialisierung zu lösen.
// In der Funktion sollte das Paket nicht erstellt werden, weil es sonst nach dem Ende der Funktion
// auf dem Stack überschrieben werden könnte. Außerhalb der Funktion können wir das Paket wegen
// Object Slicing nicht erstellen. Alle Pakettypen zu einem zusammenführen ist auch nicht praktisch,
// weil wir dann keine Möglichkeit haben, unterschiedliche Paketstrukturen zu definieren. Also sind
// drei global erstellte Pakete im Moment die einzige (praktische) Lösung. Im Moment sollte das kein
// Problem sein, weil wir nicht mehrere Pakete eines Typs gleichzeitig verwenden und sie auch nicht
// abspeichern, um sie in der Zukunft zu benutzen.
TRIA_Ping cached_ping;
TRIA_DataPong cached_pong;
TRIA_MeasureReport cached_report;

bool deserialise_packet(uint8_t *nw_bytes, uint8_t received_length, TRIA_ID &receiver_id,
                        TRIA_GenericPacket **out) {
  if (received_length < TRIA_Action::PACKED_SIZE + TRIA_ID::PACKED_SIZE * 2 ||
      received_length > TRIA_GenericPacket::PACKED_SIZE) {
    return false;
  }

  TRIA_Action a;
  a.initialise_from_buffer(nw_bytes);

  switch (a.value()) {
    case ping:
      if (received_length != TRIA_Ping::PACKED_SIZE) {
        return false;
      }
      break;
    case data_pong:
      if (received_length != TRIA_DataPong::PACKED_SIZE) {
        return false;
      }
      break;
    case measure_report:
      if (received_length != TRIA_MeasureReport::PACKED_SIZE) {
        return false;
      }
      break;
    default: VERIFY_NOT_REACHED();
  }

  TRIA_ID receive_mask;
  receive_mask.initialise_from_buffer(nw_bytes + TRIA_Action::PACKED_SIZE + TRIA_ID::PACKED_SIZE);
  if (!receiver_id.matches_mask(receive_mask)) {
    return false;
  }

  switch (a.value()) {
    case ping: *out = &cached_ping; break;
    case data_pong: *out = &cached_pong; break;
    case measure_report: *out = &cached_report; break;
  }
  (*out)->initialise_from_buffer(nw_bytes);

  return true;
}
