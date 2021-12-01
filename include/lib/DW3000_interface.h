#pragma once

#include <lib/pin_layout.h>
#include <lib/TRIA_Packet.h>
#include <lib/DW3000_DECA_API/Src/decadriver/deca_device_api.h>
#include <SPI.h>

class DW3000_Interface {
    void reset_DW3000();
    void configure_message_reception();
    void configure_message_transmission();
  public:
    DW3000_Interface() {
      DW3000_SPI_Settings = SPISettings(10000000, MSBFIRST, SPI_MODE0);
      reset_DW3000();
      configure_message_reception();
      configure_message_transmission();
    };

    TRIA_Packet receive_packet();
    void send_packet(TRIA_Packet& packet);
};