#include <Arduino.h>
#include <SPI.h>
#include <decadriver/deca_regs.h>
#include <lib/assertions.h>
#include <platform/deca_spi.h>
#include <src/DW3000_interface.h>

DW3000_Interface::DW3000_Interface(TRIA_ID &id, void (*tx_handler)(const dwt_cb_data_t *cb_data),
                                   void (*recv_handler)(const dwt_cb_data_t *cb_data)) {
  m_id = id;
  // Reihenfolge ist hier wichtig, nicht ändern!
  DWIC_reset();
  DWIC_configure_spi(SPI_FASTRATE);
  DWIC_configure_interrupts(tx_handler, recv_handler);
}

void DW3000_Interface::save_rx_stamp() {
  dwt_readrxtimestamp(m_stamp_buffer);
  m_rx_stamp.initialise_from_buffer_no_bswap(m_stamp_buffer);
}

void DW3000_Interface::save_tx_stamp() {
  dwt_readtxtimestamp(m_stamp_buffer);
  m_tx_stamp.initialise_from_buffer_no_bswap(m_stamp_buffer);
}

bool DW3000_Interface::handle_incoming_packet(size_t received_bytes, TRIA_RangeReport &out) {
  VERIFY(received_bytes - FCS_LEN <= TRIA_GenericPacket::PACKED_SIZE);
  dwt_readrxdata(m_packet_buffer, received_bytes - FCS_LEN, 0);

  TRIA_Action a;
  a.initialise_from_buffer(m_packet_buffer);
  VERIFY(a.value() != range_report);

  switch (a.value()) {
    case range_request: VERIFY(received_bytes - FCS_LEN == TRIA_RangeRequest::PACKED_SIZE); break;
    case range_response: VERIFY(received_bytes - FCS_LEN == TRIA_RangeResponse::PACKED_SIZE); break;
    default: VERIFY_NOT_REACHED();
  }

  TRIA_ID receive_mask;
  receive_mask.initialise_from_buffer(m_packet_buffer + TRIA_Action::PACKED_SIZE +
                                      TRIA_ID::PACKED_SIZE);
  if (!m_id.matches_mask(receive_mask)) {
    dwt_write32bitreg(SYS_STATUS_ID,
                      SYS_STATUS_ALL_RX_GOOD | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
    return false;
  }

  save_rx_stamp();

  TRIA_GenericPacket *received;
  switch (a.value()) {
    case range_request: received = &m_cached_range_request; break;
    case range_response: received = &m_cached_range_response; break;
    default: VERIFY_NOT_REACHED();
  }

  received->initialise_from_buffer(m_packet_buffer);
  dwt_write32bitreg(SYS_STATUS_ID,
                    SYS_STATUS_ALL_RX_GOOD | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);

  if (received->is_type(range_request)) {
    auto response = TRIA_RangeResponse(m_id, received->received_from(), m_rx_stamp);
    send_packet(&response);
    return false;
  } else {
    TRIA_Stamp measured_rx = static_cast<TRIA_RangeResponse *>(received)->get_rx_stamp();
    TRIA_Stamp measured_tx = static_cast<TRIA_RangeResponse *>(received)->get_tx_stamp();
    m_rx_stamp = m_rx_stamp - (measured_tx - measured_rx);
    out = TRIA_RangeReport(received->received_from(), m_id, m_rx_stamp, m_tx_stamp);
    return true;
  }
}

// equivalent zu handle_incoming_packet(), außer dass Daten nicht
// aus dem rx buffer eingelesen werden, sondern die Daten benutzt
// werden, die der vorherige send_packet() call in m_packet_buffer
// geschrieben hat. Außerdem wird die Systemzeit als rx stamp benutzt.
bool DW3000_Interface::receive_packet_mock(size_t received_bytes, TRIA_RangeReport &out) {
  VERIFY(received_bytes - FCS_LEN <= TRIA_GenericPacket::PACKED_SIZE);

  TRIA_Action a;
  a.initialise_from_buffer(m_packet_buffer);
  VERIFY(a.value() != range_report);

  switch (a.value()) {
    case range_request: VERIFY(received_bytes - FCS_LEN == TRIA_RangeRequest::PACKED_SIZE); break;
    case range_response: VERIFY(received_bytes - FCS_LEN == TRIA_RangeResponse::PACKED_SIZE); break;
    default: VERIFY_NOT_REACHED();
  }

  TRIA_ID receive_mask;
  receive_mask.initialise_from_buffer(m_packet_buffer + TRIA_Action::PACKED_SIZE +
                                      TRIA_ID::PACKED_SIZE);
  if (!m_id.matches_mask(receive_mask)) {
    dwt_write32bitreg(SYS_STATUS_ID,
                      SYS_STATUS_ALL_RX_GOOD | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
    return false;
  }

  uint64_t mocked_recv = static_cast<uint64_t>(dwt_readsystimestamphi32()) << 8;
  m_rx_stamp.initialise_from_buffer_no_bswap((uint8_t *)(&mocked_recv));

  TRIA_GenericPacket *received = nullptr;
  switch (a.value()) {
    case range_request: received = &m_cached_range_request; break;
    case range_response: received = &m_cached_range_response; break;
    default: VERIFY_NOT_REACHED();
  }
  received->initialise_from_buffer(m_packet_buffer);
  dwt_write32bitreg(SYS_STATUS_ID,
                    SYS_STATUS_ALL_RX_GOOD | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);

  Serial.print("Habe Paket bekommen: ");
  received->print();
  Serial.print("\n");

  if (received->is_type(range_request)) {
    Serial.println("Paket ist eine Range Request, verschicke Antwort mit rx und tx.");
    auto response = TRIA_RangeResponse(m_id, received->received_from(), m_rx_stamp);
    send_packet(&response);
    return false;
  } else {
    Serial.println("Paket ist eine Range Response, passe ToF anhand gesendetem rx und tx an.");
    TRIA_Stamp measured_rx = static_cast<TRIA_RangeResponse *>(received)->get_rx_stamp();
    TRIA_Stamp measured_tx = static_cast<TRIA_RangeResponse *>(received)->get_tx_stamp();
    m_rx_stamp = m_rx_stamp - (measured_tx - measured_rx);
    out = TRIA_RangeReport(received->received_from(), m_id, m_rx_stamp, m_tx_stamp);
    return true;
  }
}

void DW3000_Interface::send_packet(TRIA_GenericPacket *packet) {
#ifdef DEBUG
  unsigned long start_us = micros();
#endif

  VERIFY(!packet->is_type(range_report));
  // TODO: Wird das hier überhaupt gebraucht?
  while (dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS_BIT_MASK) {}

  if (packet->is_type(range_request)) {
    VERIFY(packet->packed_size() == TRIA_RangeRequest::PACKED_SIZE);
#ifdef DEBUG
    Serial.println("Paket ist eine Range Request, versende im Immediate Modus.");
#endif
    packet->pack_into(m_packet_buffer);
    VERIFY(dwt_writetxdata(packet->packed_size(), m_packet_buffer, 0) == DWT_SUCCESS);
    dwt_writetxfctrl(packet->packed_size() + FCS_LEN, 0, 0);

    dwt_starttx(DWT_START_TX_IMMEDIATE);
    dwt_readtxtimestamp(m_stamp_buffer);
    m_tx_stamp.initialise_from_buffer_no_bswap(m_stamp_buffer);
  } else if (packet->is_type(range_response)) {
    VERIFY(packet->packed_size() == TRIA_RangeResponse::PACKED_SIZE);
#ifdef DEBUG
    Serial.println("Paket ist eine Range Response, versende im Delayed Modus, damit Absendezeit in "
                   "Paket eingetragen werden kann.");
#endif
    // FIXME: Warum ist TX hier kleiner als RX?
    uint16_t antenna_delay = dwt_read16bitoffsetreg(TX_ANTD_ID, 0);
    uint32_t sys_time_hi32 = dwt_readsystimestamphi32();
    // FIXME: möglicher overflow?
    uint32_t send_time_hi32 = sys_time_hi32 + SEND_DELAY;
    auto tx = TRIA_Stamp((send_time_hi32 << 8) + antenna_delay);
    (static_cast<TRIA_RangeResponse *>(packet))->set_tx_stamp(tx);

    packet->pack_into(m_packet_buffer);
    VERIFY(dwt_writetxdata(packet->packed_size(), m_packet_buffer, 0) == DWT_SUCCESS);
    dwt_writetxfctrl(packet->packed_size() + FCS_LEN, 0, 0);
#ifdef DEBUG
    Serial.printf("Sende um %u, Systemzeit (hi32) ist im Moment %u.\n", send_time_hi32,
                  dwt_readsystimestamphi32());
#endif
    // FIXME: Könnte Problem geben, wenn systimer zwischen Berechnungen auf 0 zurückgesetzt wird.
    // Aber ka ob das überhaupt passieren kann.
    VERIFY(send_time_hi32 > dwt_readsystimestamphi32());
    dwt_setdelayedtrxtime(send_time_hi32);
    dwt_starttx(DWT_START_TX_DELAYED);
  }

#ifdef DEBUG
  Serial.print("Versendetes Paket: ");
  packet->print();
  Serial.print("\n");

  Serial.printf("Netzwerkrepräsentation (%u Bytes):", packet->packed_size());
  for (size_t i = 0; i < packet->packed_size(); i++) {
    Serial.print(" 0x");
    Serial.print(m_packet_buffer[i], HEX);
  }
  Serial.print("\n");

  unsigned long end_us = micros();
  Serial.printf("Benchmark = %uus\n\n", end_us - start_us);
#endif
}