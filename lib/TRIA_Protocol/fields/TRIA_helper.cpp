#include <Arduino.h>
#include <fields/TRIA_helper.h>


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
