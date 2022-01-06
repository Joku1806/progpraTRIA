#include <Arduino.h>
#include <SPI.h>
#include <decadriver/deca_regs.h>
#include <lib/assertions.h>
#include <platform/deca_spi.h>
#include <src/DW3000_interface.h>

DW3000_Interface::DW3000_Interface(TRIA_ID &id, void (*recv_handler)(const dwt_cb_data_t *cb_data)) {
  m_id = id;
  // Reihenfolge ist hier wichtig, nicht ändern!
  DWIC_reset();
  DWIC_configure_spi(SPI_FASTRATE);
  DWIC_configure_interrupts(recv_handler);
}

bool DW3000_Interface::handle_incoming_packet(size_t received_bytes, TRIA_RangeReport &out) {
  VERIFY(received_bytes - FCS_LEN <= TRIA_GenericPacket::PACKED_SIZE);
  dwt_readrxdata(m_packet_buffer, received_bytes - FCS_LEN, 0);

  TRIA_Action a;
  a.initialise_from_buffer(m_packet_buffer);
  VERIFY(a.value() != range_report);

  switch (a.value()) {
    case range_request: VERIFY(received_bytes - FCS_LEN == TRIA_RangeRequest::PACKED_SIZE);
    case range_response: VERIFY(received_bytes - FCS_LEN == TRIA_RangeResponse::PACKED_SIZE);
    default: VERIFY_NOT_REACHED();
  }

  TRIA_ID receive_mask;
  receive_mask.initialise_from_buffer(m_packet_buffer + TRIA_Action::PACKED_SIZE + TRIA_ID::PACKED_SIZE);
  if (!m_id.matches_mask(receive_mask)) {
    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_GOOD | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
    return false;
  }

  dwt_readrxtimestamp(m_stamp_buffer);
  m_rx_stamp.initialise_from_buffer(m_stamp_buffer);

  TRIA_GenericPacket *received;
  switch (a.value()) {
    case range_request: received = &m_cached_range_request;
    case range_response: received = &m_cached_range_response;
    default: VERIFY_NOT_REACHED();
  }

  received->initialise_from_buffer(m_packet_buffer);
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_GOOD | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);

  if (received->is_type(range_request)) {
    auto response = TRIA_RangeResponse(received->received_from(), m_id, m_rx_stamp);
    send_packet(response);
    return false;
  } else {
    // FIXME: irgendwie ohne hässlichen Cast hinkriegen
    TRIA_Stamp measured_rx = ((TRIA_RangeResponse *)received)->get_rx_stamp();
    TRIA_Stamp measured_tx = ((TRIA_RangeResponse *)received)->get_tx_stamp();
    m_rx_stamp = m_rx_stamp - (measured_tx - measured_rx);
    out = TRIA_RangeReport(received->received_from(), m_id, m_rx_stamp, m_tx_stamp);
    return true;
  }
}

void DW3000_Interface::send_packet(TRIA_GenericPacket &packet) {
  VERIFY(!packet.is_type(range_report));
  // TODO: Wird das hier überhaupt gebraucht?
  while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS_BIT_MASK)) {}
  Serial.println("Sende Packet:");
  packet.print();

  packet.pack_into(m_packet_buffer);
  VERIFY(dwt_writetxdata(packet.packed_size(), m_packet_buffer, 0) == DWT_SUCCESS);
  dwt_writetxfctrl(packet.packed_size() + FCS_LEN, 0, 0);

  if (packet.is_type(range_request)) {
    dwt_starttx(DWT_START_TX_IMMEDIATE);
    dwt_readtxtimestamp(m_stamp_buffer);
    m_tx_stamp.initialise_from_buffer(m_stamp_buffer);
  } else if (packet.is_type(range_response)) {
    uint16_t antenna_delay = dwt_read16bitoffsetreg(TX_ANTD_ID, 0);
    uint32_t sys_time = dwt_readsystimestamphi32();
    uint32_t send_time_hi32 = sys_time + SEND_DELAY;
    auto tx = TRIA_Stamp((send_time_hi32 << 8) + antenna_delay);
    ((TRIA_RangeResponse *)&packet)->set_tx_stamp(tx);
    VERIFY(send_time_hi32 < dwt_readsystimestamphi32());
    dwt_setdelayedtrxtime(send_time_hi32);
    dwt_starttx(DWT_START_TX_DELAYED);
  }

  // FIXME: Kann es passieren, dass etwas gesendet wird, während delayed tx noch aktiv ist?
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_TX);
}