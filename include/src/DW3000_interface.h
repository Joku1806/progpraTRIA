#pragma once

#include <Ringbuffer.h>
#include <SPI.h>
#include <algorithm>
#include <decadriver/deca_device_api.h>
#include <packets/TRIA_GenericPacket.h>
#include <packets/TRIA_MeasureReport.h>
#include <packets/TRIA_RangeRequest.h>
#include <packets/TRIA_RangeResponse.h>

struct BinaryMessage {
public:
  uint64_t receive_time {0};
  uint8_t data[TRIA_GenericPacket::PACKED_SIZE];
  size_t data_length {0};

  BinaryMessage() {}
  ~BinaryMessage() {}

  BinaryMessage &operator=(const BinaryMessage &other) {
    receive_time = other.receive_time;
    std::copy(other.data, other.data + other.data_length, data);
    data_length = other.data_length;

    return *this;
  }
};

class DW3000_Interface {
public:
  static const size_t SPI_SLOWRATE = 4500000;
  static const size_t SPI_FASTRATE = 10000000;
  // in Einheit von 8ns, siehe API Guide S. 44
  // Benchmark von Delayed TX Code ist max. 1700us (1600us + slack)
  // => 1700000ns => 212500 delay
  static const uint32_t SEND_DELAY = 212500;
  // delay, damit RangeResponses nicht alle gleichzeitig beim Trackee ankommen.
  // Benchmark von RX Interrupt Routine ist max. 200us + Zeit, die zwischen dwt_writefastcmd(CMD_RX)
  // und dem wirklichen Anschalten des Receivers liegt
  // => 200000ns => 25000 delay
  static const uint32_t SLOT_DELAY = 175000;

  DW3000_Interface() {};
  DW3000_Interface(TRIA_ID &id, void (*recv_handler)(const dwt_cb_data_t *cb_data));

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

  TRIA_Stamp m_tx_stamp;
  Ringbuffer<BinaryMessage, 16> m_store;
  uint8_t m_assigned_slot;
};