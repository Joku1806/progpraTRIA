#pragma once

#include <SPI.h>
#include <lib/DW3000_DECA_API/Src/decadriver/deca_device_api.h>
#include <lib/packets/TRIA_GenericPacket.h>
#include <lib/packets/TRIA_RangeReport.h>
#include <lib/packets/TRIA_RangeRequest.h>
#include <lib/packets/TRIA_RangeResponse.h>
#include <lib/pin_layout.h>
#include <optional>

class DW3000_Interface {
public:
  DW3000_Interface(TRIA_ID &id, void (*recv_handler)(const dwt_cb_data_t *cb_data));

  std::optional<TRIA_RangeReport> handle_incoming_packet(size_t received_bytes);
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