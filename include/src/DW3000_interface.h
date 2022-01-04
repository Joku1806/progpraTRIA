#pragma once

#include <SPI.h>
#include <decadriver/deca_device_api.h>
#include <packets/TRIA_GenericPacket.h>
#include <packets/TRIA_RangeReport.h>
#include <packets/TRIA_RangeRequest.h>
#include <packets/TRIA_RangeResponse.h>

class DW3000_Interface {
public:
  static const size_t SPI_SLOWRATE = 4500000;
  static const size_t SPI_FASTRATE = 38000000;

  DW3000_Interface(TRIA_ID &id, void (*recv_handler)(const dwt_cb_data_t *cb_data));

  bool handle_incoming_packet(size_t received_bytes, TRIA_RangeReport &out);
  void send_packet(TRIA_GenericPacket &packet);

private:
  TRIA_ID m_id;

  uint8_t m_packet_buffer[TRIA_GenericPacket::PACKED_SIZE];
  uint8_t m_stamp_buffer[TRIA_Stamp::PACKED_SIZE];

  TRIA_Stamp m_rx_stamp;
  TRIA_Stamp m_tx_stamp;
  TRIA_RangeRequest m_cached_range_request;
  TRIA_RangeResponse m_cached_range_response;
};