#pragma once

#include <BinaryMessageStore.h>
#include <SPI.h>
#include <decadriver/deca_device_api.h>
#include <packets/TRIA_GenericPacket.h>
#include <packets/TRIA_RangeReport.h>
#include <packets/TRIA_RangeRequest.h>
#include <packets/TRIA_RangeResponse.h>

class DW3000_Interface {
public:
  static const size_t SPI_SLOWRATE = 4500000;
  static const size_t SPI_FASTRATE = 10000000;
  // so groß, dass das VERIFY in send_packet()
  // nicht mehr getriggert wird.
  // in Einheit von 8ns, siehe API Guide S. 44
  // Benchmark von Delayed TX Code ist max. 800us
  // => 800000ns => 100000 delay
  static const uint32_t SEND_DELAY = 100000;

  DW3000_Interface() {};
  DW3000_Interface(void (*recv_handler)(const dwt_cb_data_t *cb_data));

  void save_tx_stamp();
  TRIA_Stamp get_tx_stamp();

  void store_received_message(const dwt_cb_data_t *cb_data);
  bool unprocessed_messages_pending();
  BinaryMessage get_first_unprocessed_message();

  void send_range_request(TRIA_RangeRequest &request);
  void send_range_response(TRIA_RangeResponse &response);

private:
  uint8_t m_packet_buffer[TRIA_GenericPacket::PACKED_SIZE];
  uint8_t m_stamp_buffer[TRIA_Stamp::PACKED_SIZE];

  TRIA_Stamp m_rx_stamp;
  TRIA_Stamp m_tx_stamp;
  TRIA_RangeRequest m_cached_range_request;
  TRIA_RangeResponse m_cached_range_response;

  BinaryMessageStore m_store;
};