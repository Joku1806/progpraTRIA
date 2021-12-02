#include <lib/DW3000_interface.h>
#include <lib/DW3000_DECA_API/Src/decadriver/deca_regs.h>
#include <Arduino.h>

DW3000_Interface::DW3000_Interface(TRIA_ID& id, void (*recv_handler)(const dwt_cb_data_t *cb_data)) {
  m_id = id;
  m_spi_settings = SPISettings(10000000, MSBFIRST, SPI_MODE0);
  reset_DW3000();
  dwt_initialise(DWT_DW_INIT);

  // TODO: nachfragen ob diese Werte in Ordnung sind,
  // bzw. ob der SPI Speed runtergeschraubt werden muss
  dwt_config_t config = {
    .chan = 5,
    .txPreambLength = DWT_PLEN_32,
    .rxPAC = DWT_PAC4,
    .txCode = 3,
    .rxCode = 3,
    .sfdType = DWT_SFD_DW_16,
    .dataRate = DWT_BR_6M8,
    .phrMode = DWT_PHRMODE_STD,
    .phrRate = DWT_BR_6M8,
    .sfdTO = DWT_PLEN_32 + 1 + DWT_SFD_DW_16 - DWT_PAC4,
    .stsMode = DWT_STS_MODE_OFF,
    .stsLength = DWT_STS_LEN_32,
    .pdoaMode = DWT_PDOA_M0,
  };

  dwt_configure(&config);
  dwt_setcallbacks(nullptr, recv_handler, nullptr, nullptr, nullptr, nullptr);

  dwt_writefastCMD(CMD_RX);
}

void DW3000_Interface::reset_DW3000() {
  pinMode(SPI_reset, OUTPUT);
  digitalWrite(SPI_reset, LOW);
  delayMicroseconds(100);
  pinMode(SPI_reset, INPUT);
  delayMicroseconds(1000);
}

std::optional<TRIA_Packet> DW3000_Interface::handle_incoming_packet(size_t received_bytes) {
  if (received_bytes > TRIA_Packet::MAX_PACKET_SIZE + FCS_LEN) {
    // TODO: Fehlermeldung ausgeben
    return {};
  }

  dwt_readrxdata(rx_buffer, received_bytes - FCS_LEN, 0);
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG_BIT_MASK);

  TRIA_Packet received;
  received.initialise_from_buffer(rx_buffer);

  if (!received.is_addressed_to(m_id)) {
    return {};
  }

  TRIA_Stamp rx_stamp;
  dwt_readrxtimestamp(rx_stamp.packed());

  if (received.is_type(range_request)) {
    TRIA_Packet response = TRIA_Packet(TRIA_Action(range_response), received.get_sender_id(), m_id, rx_stamp);
    send_packet(response);
  } else if (received.is_type(range_response)) {
    rx_stamp = rx_stamp - (received.get_tx_stamp() - received.get_rx_stamp());
    return TRIA_Packet(TRIA_Action(range_report), received.get_sender_id(), m_id, rx_stamp, m_saved_tx);
  }

  return {};
}

void DW3000_Interface::send_packet(TRIA_Packet& packet) {
  while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS_BIT_MASK)) {}
  dwt_writetxdata(packet.packed_size(), packet.packed(), 0);
  dwt_writetxfctrl(packet.packed_size() + FCS_LEN, 0, 0);  
  // TODO: nachsehen ob TX_STAMP automatisch geschrieben wird
  dwt_writefastCMD(CMD_TX);

  if (packet.is_type(range_request)) {
    // diesen Hack vielleicht noch mal umschreiben :^)
    // sollte aber perfekt funktionieren, jedenfalls
    // bis die Signatur von packed() geändert wird
    dwt_readtxtimestamp(m_saved_tx.packed());
  }
}