#pragma once

#include <Arduino.h>
#include <RH_RF95.h>
#include <fields/TRIA_ID.h>
#include <lib/assertions.h>
#include <platform/pin_mappings.h>
#include <src/DW3000_interface.h>
#include <src/USB_interface.h>

void recv_interrupt_handler(const dwt_cb_data_t *cb_data);

class ComponentBridge {
public:
  ComponentBridge() {};

  void initialise(void (*dw_receive_interrupt_handler)(const dwt_cb_data_t *cb_data));

  void receive_dw_message(const dwt_cb_data_t *cb_data);
  bool did_receive_dw_messages();
  void handle_received_dw_messages();

  bool did_receive_lora_messages();
  void handle_received_lora_messages();

  bool current_measurement_finished();
  void send_current_measurement();

  bool measurement_requested();
  void start_measurement();

private:
  TRIA_ID m_id;

  uint8_t m_recv_buffer[TRIA_GenericPacket::PACKED_SIZE];
  uint8_t m_send_buffer[TRIA_GenericPacket::PACKED_SIZE];

  RH_RF95 m_rf95 = RH_RF95(8, 3);
  DW3000_Interface m_dw_interface;
  USB_Interface m_usb_interface;

  TRIA_ID build_id();
  void send_packet_over_lora(TRIA_GenericPacket &packet);
};