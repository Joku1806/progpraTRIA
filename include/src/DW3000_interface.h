#pragma once

#include <Ringbuffer.h>
#include <SPI.h>
#include <algorithm>
#include <decadriver/deca_device_api.h>
#include <packets/TRIA_DataPong.h>
#include <packets/TRIA_GenericPacket.h>
#include <packets/TRIA_MeasureReport.h>
#include <packets/TRIA_Ping.h>

struct BinaryMessage {
public:
  TRIA_Stamp rx_stamp;
  uint8_t data[TRIA_GenericPacket::PACKED_SIZE];
  size_t data_length {0};

  BinaryMessage() {}
  ~BinaryMessage() {}

  BinaryMessage &operator=(const BinaryMessage &other) {
    rx_stamp = other.rx_stamp;
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
  // Laufzeit von einem ganzen Double Sided Ranging
  // Laufzeit von RX Interrupt Routine = 200us
  // Zeit, um zwischen send- und receive mode umzuschalten: sagen wir ~2000us
  // (2000us + 200us) * 3 = 6600us => 825000 delay
  static const uint32_t SLOT_DELAY = 825000;

  DW3000_Interface() {};
  DW3000_Interface(TRIA_ID &id, void (*recv_handler)(const dwt_cb_data_t *cb_data));

  void save_tx_stamp();
  TRIA_Stamp get_tx_stamp();

  TRIA_Stamp get_timediff_D() { return m_2wrt_holder.get_timediff_D(); }
  TRIA_Stamp get_timediff_R() { return m_2wrt_holder.get_timediff_R(); }
  void set_timediff_D(TRIA_Stamp &D) { m_2wrt_holder.set_timediff_D(D); }
  void set_timediff_R(TRIA_Stamp &R) { m_2wrt_holder.set_timediff_R(R); }

  void store_received_message(const dwt_cb_data_t *cb_data);
  bool unprocessed_messages_pending();
  BinaryMessage get_first_unprocessed_message();

  void send_ping(TRIA_Ping &ping);
  void send_data_pong(TRIA_DataPong &pong, TRIA_Stamp &ping1_rx);

private:
  uint8_t m_packet_buffer[TRIA_GenericPacket::PACKED_SIZE];
  uint8_t m_stamp_buffer[TRIA_Stamp::PACKED_SIZE];

  TRIA_Stamp m_tx_stamp;
  TRIA_DataPong m_2wrt_holder;

  Ringbuffer<BinaryMessage, 16> m_store;
  uint8_t m_assigned_slot;
};