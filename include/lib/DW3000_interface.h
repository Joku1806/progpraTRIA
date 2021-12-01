#pragma once

#include <lib/pin_layout.h>
#include <lib/TRIA_Packet.h>
#include <lib/DW3000_DECA_API/Src/decadriver/deca_device_api.h>
#include <SPI.h>

class DW3000_Interface {
  public:
    DW3000_Interface();

    static void receive_packet(const dwt_cb_data_t *cb_data);
    void send_packet(TRIA_Packet& packet);
  private:
    TRIA_Stamp m_saved_tx;
    SPISettings m_spi_settings;
    void reset_DW3000();
};