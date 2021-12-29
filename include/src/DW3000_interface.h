#pragma once

#include <SPI.h>
#include <decadriver/deca_device_api.h>
#include <lib/TRIA_Protocol/packets/TRIA_GenericPacket.h>
#include <lib/TRIA_Protocol/packets/TRIA_RangeReport.h>
#include <lib/TRIA_Protocol/packets/TRIA_RangeRequest.h>
#include <lib/TRIA_Protocol/packets/TRIA_RangeResponse.h>
#include <lib/pin_layout.h>

class DW3000_Interface {
public:
  DW3000_Interface(TRIA_ID &id, void (*recv_handler)(const dwt_cb_data_t *cb_data));

  bool handle_incoming_packet(size_t received_bytes, TRIA_RangeReport &out);
  void send_packet(TRIA_GenericPacket &packet);

private:
  SPISettings m_spi_settings;
  TRIA_ID m_id;

  uint8_t m_packet_buffer[TRIA_GenericPacket::PACKED_SIZE];
  uint8_t m_stamp_buffer[TRIA_Stamp::PACKED_SIZE];

  TRIA_Stamp m_rx_stamp;
  TRIA_Stamp m_tx_stamp;
  TRIA_RangeRequest m_cached_range_request;
  TRIA_RangeResponse m_cached_range_response;

  void reset_DW3000();
};