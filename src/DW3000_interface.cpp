#include <Arduino.h>
#include <SPI.h>
#include <TRIA_helper.h>
#include <decadriver/deca_regs.h>
#include <lib/assertions.h>
#include <platform/deca_spi.h>
#include <src/DW3000_interface.h>

DW3000_Interface::DW3000_Interface(TRIA_ID &id,
                                   void (*recv_handler)(const dwt_cb_data_t *cb_data)) {
  VERIFY(id.id() != 0);
  m_assigned_slot = id.id() - 1;

  // Reihenfolge ist hier wichtig, nicht ändern!
  DWIC_reset();
  DWIC_configure_spi(SPI_FASTRATE);
  DWIC_configure_interrupts(recv_handler);

  dwt_setrxaftertxdelay(0);
  // sofort Verbindungen annehmen
  dwt_writefastCMD(CMD_RX);
}

void DW3000_Interface::store_received_message(const dwt_cb_data_t *cb_data) {
  // TODO: ist cb_datalength immer min FCS_LEN groß?
  if (cb_data->datalength - FCS_LEN > TRIA_GenericPacket::PACKED_SIZE) {
    return;
  }

  // FIXME: BinaryMessage sollte eine TRIA_Stamp() beinhalten
  BinaryMessage received;

  dwt_readrxtimestamp(m_stamp_buffer);
  received.rx_stamp.initialise_from_buffer_no_bswap(m_stamp_buffer);
  dwt_readrxdata(received.data, cb_data->datalength - FCS_LEN, 0);
  received.data_length = cb_data->datalength - FCS_LEN;

  m_store.push_back(received);

  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_GOOD);
}

bool DW3000_Interface::unprocessed_messages_pending() { return !m_store.is_empty(); }
BinaryMessage DW3000_Interface::get_first_unprocessed_message() { return m_store.pop_front(); }

void DW3000_Interface::save_tx_stamp() {
  dwt_readtxtimestamp(m_stamp_buffer);
  m_tx_stamp.initialise_from_buffer_no_bswap(m_stamp_buffer);
}

TRIA_Stamp DW3000_Interface::get_tx_stamp() { return m_tx_stamp; }

void DW3000_Interface::send_ping(TRIA_Ping &ping) {
  dwt_forcetrxoff();

  ping.pack_into(m_packet_buffer);
  VERIFY(dwt_writetxdata(TRIA_Ping::PACKED_SIZE, m_packet_buffer, 0) == DWT_SUCCESS);
  dwt_writetxfctrl(TRIA_Ping::PACKED_SIZE + FCS_LEN, 0, 0);

  if (ping.is_first()) {
    uint32_t sys_time_hi32 = dwt_readsystimestamphi32();
    uint64_t send_time_hi32 = sys_time_hi32 + SEND_DELAY + m_assigned_slot * SLOT_DELAY;

    dwt_setdelayedtrxtime(send_time_hi32);
    VERIFY(dwt_starttx(DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED) == DWT_SUCCESS);
  } else {
    VERIFY(dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED) == DWT_SUCCESS);
  }

  while (!(dwt_read8bitoffsetreg(SYS_STATUS_ID, 0) & SYS_STATUS_TXFRS_BIT_MASK)) {};
  save_tx_stamp();
  dwt_write8bitoffsetreg(SYS_STATUS_ID, 0, SYS_STATUS_TXFRS_BIT_MASK);

#ifdef DEBUG_ON
  Serial.print("Paket gesendet (DW): ");
  ping.print();
  Serial.print("\n");
#endif
}

void DW3000_Interface::send_data_pong(TRIA_DataPong &response, TRIA_Stamp &ping1_rx) {
  dwt_forcetrxoff();

  BENCHMARK(uint16_t antenna_delay = dwt_read16bitoffsetreg(TX_ANTD_ID, 0);
            uint32_t sys_time_hi32 = dwt_readsystimestamphi32();
            uint64_t send_time_hi32 = sys_time_hi32 + SEND_DELAY;

            auto timediff_R = ping1_rx - get_tx_stamp();
            auto timediff_D = TRIA_Stamp((send_time_hi32 << 8) + antenna_delay) - ping1_rx;
            response.set_timediff_D(timediff_D); response.set_timediff_R(timediff_R);

            response.pack_into(m_packet_buffer);

            VERIFY(dwt_writetxdata(TRIA_DataPong::PACKED_SIZE, m_packet_buffer, 0) == DWT_SUCCESS);
            dwt_writetxfctrl(TRIA_DataPong::PACKED_SIZE + FCS_LEN, 0, 0);
            dwt_setdelayedtrxtime(send_time_hi32);

            VERIFY(dwt_starttx(DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED) == DWT_SUCCESS);

            while (!(dwt_read8bitoffsetreg(SYS_STATUS_ID, 0) & SYS_STATUS_TXFRS_BIT_MASK)) {};);

  dwt_write8bitoffsetreg(SYS_STATUS_ID, 0, SYS_STATUS_TXFRS_BIT_MASK);

#ifdef DEBUG_ON
  Serial.print("Paket gesendet (DW): ");
  response.print();
  Serial.print("\n");
#endif
}