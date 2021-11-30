#pragma once

#include <lib/pin_layout.h>
#include <lib/TRIA_Packet.h>
#include <lib/SPI_commands.h>
#include <SPI.h>

class DW3000_Interface {
  private:
    SPISettings DW3000_SPI_Settings;
    uint8_t get_fast_command_transaction_header(SPI_command cmd);
    uint8_t get_short_addressed_transaction_header(transaction_type type, uint8_t base);
    uint16_t get_full_addressed_transaction_header(transaction_type type, uint8_t base, uint8_t sub);
    uint16_t get_masked_write_transaction_header(uint8_t base, uint8_t sub, transaction_mode mode);
    
    void execute_nonreturning_SPI_command(uint8_t cmd);
    uint64_t DW3000_Interface::get_sys_status();

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