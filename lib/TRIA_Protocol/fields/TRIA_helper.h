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

  measure_counter++;
  measure_received = true;
}
