#pragma once

#include <optional>
#include <lib/pin_layout.h>
#include <lib/TRIA_Packet.h>
#include <lib/DW3000_DECA_API/Src/decadriver/deca_device_api.h>
#include <SPI.h>

class DW3000_Interface {
  public:
    DW3000_Interface(TRIA_ID& id, void (*recv_handler)(const dwt_cb_data_t *cb_data));

    std::optional<TRIA_Packet> handle_incoming_packet(size_t received_bytes);
    void send_packet(TRIA_Packet& packet);
  private:
    TRIA_ID m_id;
    TRIA_Stamp m_saved_tx;
    uint8_t rx_buffer[TRIA_Packet::MAX_PACKET_SIZE];
    SPISettings m_spi_settings;
    void reset_DW3000();
};