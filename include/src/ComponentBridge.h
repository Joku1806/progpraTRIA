#pragma once

#include <Arduino.h>
#include <RH_RF95.h>
#include <fields/TRIA_ID.h>
#include <packets/TRIA_MeasureReport.h>
#include <src/DW3000_interface.h>
#include <src/USB_interface.h>

class ComponentBridge {
public:
  ComponentBridge() {};

  void initialise(void (*dw_receive_interrupt_handler)(const dwt_cb_data_t *cb_data));
  void execute();

  void receive_dw_message(const dwt_cb_data_t *cb_data);
  void process_new_dw_messages();
  void process_new_lora_messages();

private:
  TRIA_ID m_id;

  uint8_t m_recv_buffer[TRIA_GenericPacket::PACKED_SIZE];
  uint8_t m_send_buffer[TRIA_GenericPacket::PACKED_SIZE];

  RH_RF95 m_rf95 = RH_RF95(8, 3);
  DW3000_Interface m_dw_interface;
  USB_Interface m_usb_interface;
  TRIA_MeasureReport m_report;

  TRIA_ID build_id();
  void send_packet_over_lora(TRIA_GenericPacket &packet);
};