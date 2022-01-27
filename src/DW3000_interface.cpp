#include <Arduino.h>
#include <SPI.h>
#include <TRIA_helper.h>
#include <decadriver/deca_regs.h>
#include <lib/assertions.h>
#include <platform/deca_spi.h>
#include <src/DW3000_interface.h>

DW3000_Interface::DW3000_Interface(TRIA_ID &id,
                                   void (*recv_handler)(const dwt_cb_data_t *cb_data)) {
  m_id = id;
  // Reihenfolge ist hier wichtig, nicht ändern!
  DWIC_reset();
  DWIC_configure_spi(SPI_FASTRATE);
  DWIC_configure_interrupts(recv_handler);
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
  dwt_readrxdata(m_packet_buffer, received_bytes - FCS_LEN, 0);
  if (!packet_ok(m_packet_buffer, received_bytes - FCS_LEN, m_id)) {
#ifdef DEBUG
    Serial.println("Empfangenes Paket hat falsches Format oder ist nicht an mich adressiert.");
    Serial.print("Netzwerkrepräsentation:");

    for (size_t i = 0; i < received_bytes - FCS_LEN; i++) {
      Serial.print(" 0x");
      Serial.print(m_packet_buffer[i], HEX);
    }
    Serial.print("\n");
#endif

    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_GOOD);
    return false;
  }

  save_rx_stamp();

  TRIA_GenericPacket *received;
  TRIA_Action action;
  action.initialise_from_buffer(m_packet_buffer);

  switch (action.value()) {
    case range_request: received = &m_cached_range_request; break;
    case range_response: received = &m_cached_range_response; break;
    default: VERIFY_NOT_REACHED();
  }

  received->initialise_from_buffer(m_packet_buffer);
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_GOOD);

#ifdef DEBUG
  Serial.print("Paket empfangen: ");
  received->print();
  Serial.print("\n");
#endif

  if (received->is_type(range_request)) {
    auto response = TRIA_RangeResponse(m_id, received->received_from(), m_rx_stamp);
    send_packet(response);
    return false;
  } else if (received->is_type(range_report)) {
    TRIA_Stamp measured_rx = static_cast<TRIA_RangeResponse *>(received)->get_rx_stamp();
    TRIA_Stamp measured_tx = static_cast<TRIA_RangeResponse *>(received)->get_tx_stamp();
    m_rx_stamp = m_rx_stamp - (measured_tx - measured_rx);
    out = TRIA_RangeReport(received->received_from(), m_id, m_rx_stamp, m_tx_stamp);
    return true;
  }
}

void DW3000_Interface::send_packet(TRIA_GenericPacket &packet) {
  VERIFY(!packet.is_type(range_report));
  // Wir wissen nicht, ob wir diese Funktion in einem Interrupt oder im normalen
  // Programmablauf aufrufen, deswegen gehen wir von dem Fall aus, der etwas kaputtmachen könnte,
  // d.h. ein send() aus dem Interruptmodus unterbricht ein normales send(). Deswegen schalten wir
  // direkt nach diesem Aufruf (der Interrupts wieder anschaltet), Interrupts wieder aus, damit das
  // nicht vorkommen kann. Falls es trotzdem irgendwelche Probleme gibt, bleibt uns nicht anderes
  // übrig, als zwei separate Sendefunktionen zu haben.
  dwt_forcetrxoff();
  decaIrqStatus_t stat = decamutexon();

  if (packet.is_type(range_request)) {
    VERIFY(packet.packed_size() == TRIA_RangeRequest::PACKED_SIZE);
    packet.pack_into(m_packet_buffer);
    VERIFY(dwt_writetxdata(packet.packed_size(), m_packet_buffer, 0) == DWT_SUCCESS);
    dwt_writetxfctrl(packet.packed_size() + FCS_LEN, 0, 0);
    VERIFY(dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED) == DWT_SUCCESS);
  } else if (packet.is_type(range_response)) {
    VERIFY(packet.packed_size() == TRIA_RangeResponse::PACKED_SIZE);
    uint16_t antenna_delay = dwt_read16bitoffsetreg(TX_ANTD_ID, 0);
    uint32_t sys_time_hi32 = dwt_readsystimestamphi32();
    uint64_t send_time_hi32 = sys_time_hi32 + SEND_DELAY;
    auto tx = TRIA_Stamp((send_time_hi32 << 8) + antenna_delay);
    (static_cast<TRIA_RangeResponse &>(packet)).set_tx_stamp(tx);

    packet.pack_into(m_packet_buffer);
    VERIFY(dwt_writetxdata(packet.packed_size(), m_packet_buffer, 0) == DWT_SUCCESS);
    dwt_writetxfctrl(packet.packed_size() + FCS_LEN, 0, 0);
    dwt_setdelayedtrxtime(send_time_hi32);
    VERIFY(dwt_starttx(DWT_START_TX_DELAYED) == DWT_SUCCESS);
  }

  while (!(dwt_read8bitoffsetreg(SYS_STATUS_ID, 0) & SYS_STATUS_TXFRS_BIT_MASK)) {};
  save_tx_stamp();
  dwt_write8bitoffsetreg(SYS_STATUS_ID, 0, SYS_STATUS_TXFRS_BIT_MASK);

#ifdef DEBUG
  Serial.print("Paket gesendet: ");
  packet->print();
  Serial.print("\n");
#endif

  dwt_forcetrxoff();
  // Default Modus ist receive, deswegen nach Senden wieder direkt dahin zurückschalten
  dwt_writefastCMD(CMD_RX);
  decamutexoff(stat);
}