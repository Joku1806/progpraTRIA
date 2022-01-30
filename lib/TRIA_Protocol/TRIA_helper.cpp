#include <Arduino.h>
#include <TRIA_helper.h>
#include <lib/assertions.h>
#include <packets/TRIA_RangeReport.h>
#include <packets/TRIA_RangeRequest.h>
#include <packets/TRIA_RangeResponse.h>

// FIXME: Sehr hässliche Art, das Problem der Paket Initialisierung zu lösen.
// In der Funktion sollte das Paket nicht erstellt werden, weil es sonst nach dem Ende der Funktion
// auf dem Stack überschrieben werden könnte. Außerhalb der Funktion können wir das Paket wegen
// Object Slicing nicht erstellen. Alle Pakettypen zu einem zusammenführen ist auch nicht praktisch,
// weil wir dann keine Möglichkeit haben, unterschiedliche Paketstrukturen zu definieren. Also sind
// drei global erstellte Pakete im Moment die einzige (praktische) Lösung. Im Moment sollte das kein
// Problem sein, weil wir nicht mehrere Pakete eines Typs gleichzeitig verwenden und sie auch nicht
// abspeichern, um sie in der Zukunft zu benutzen.
TRIA_RangeRequest cached_request;
TRIA_RangeResponse cached_response;
TRIA_RangeReport cached_report;

bool deserialise_packet(uint8_t *nw_bytes, uint8_t received_length, TRIA_ID &receiver_id,
                        TRIA_GenericPacket **out) {
  if (received_length < TRIA_Action::PACKED_SIZE + TRIA_ID::PACKED_SIZE * 2 ||
      received_length > TRIA_GenericPacket::PACKED_SIZE) {
    return false;
  }

  TRIA_Action a;
  a.initialise_from_buffer(nw_bytes);

  switch (a.value()) {
    case range_request:
      if (received_length != TRIA_RangeRequest::PACKED_SIZE) {
        return false;
      }
      break;
    case range_response:
      if (received_length != TRIA_RangeResponse::PACKED_SIZE) {
        return false;
      }
      break;
    case range_report:
      if (received_length != TRIA_RangeReport::PACKED_SIZE) {
        return false;
      }
      break;
    default: VERIFY_NOT_REACHED();
  }

  // FIXME: Range Reports sollten stattdessen das gesamte Paket einbetten
  // und eine eigene Receiver/Sender ID haben, dann muss man nicht diesen
  // Hack machen.
  if (receiver_id.is_coordinator() && a.value() == range_report) {
    *out = &cached_report;
    (*out)->initialise_from_buffer(nw_bytes);

    return true;
  }

  TRIA_ID receive_mask;
  receive_mask.initialise_from_buffer(nw_bytes + TRIA_Action::PACKED_SIZE + TRIA_ID::PACKED_SIZE);
  if (!receiver_id.matches_mask(receive_mask)) {
    return false;
  }

  switch (a.value()) {
    case range_request: *out = &cached_request; break;
    case range_response: *out = &cached_response; break;
    case range_report: *out = &cached_report; break;
  }
  (*out)->initialise_from_buffer(nw_bytes);

  return true;
}
