#include <lib/DW3000_interface.h>
#include <Arduino.h>

void DW3000_Interface::reset_DW3000() {
  pinMode(SPI_reset, OUTPUT);
  digitalWrite(SPI_reset, LOW);
  delayMicroseconds(100);
  pinMode(SPI_reset, INPUT);
  delayMicroseconds(1000);
}

void DW3000_Interface::execute_nonreturning_SPI_command(uint8_t cmd) {
  SPI.beginTransaction(this->DW3000_SPI_Settings);
  digitalWrite(SPI_chipselect, LOW);

  SPI.transfer(cmd);

  digitalWrite(SPI_chipselect, HIGH);
  SPI.endTransaction();
}

uint8_t DW3000_Interface::get_fast_command_transaction_header(SPI_command cmd) {
  return (1 << 7) | cmd & 0x3e | (1 << 0);
}

uint8_t DW3000_Interface::get_short_addressed_transaction_header(transaction_type type, uint8_t base) {
  return (type << 7) | base & 0x3e;
}

uint16_t DW3000_Interface::get_full_addressed_transaction_header(transaction_type type, uint8_t base, uint8_t sub) {
  return (type << 14) | (base & 0x1f) << 8 | (sub & 0x7f) << 2 | transaction_RD_RW;
}

uint16_t DW3000_Interface::get_masked_write_transaction_header(uint8_t base, uint8_t sub, transaction_mode mode) {
  return (3 << 14) | (base & 0x1f) << 8 | (sub & 0x7f) << 2 | mode;
}

uint64_t DW3000_Interface::get_sys_status() {
  SPI.beginTransaction(this->DW3000_SPI_Settings);
  digitalWrite(SPI_chipselect, LOW);

  uint64_t status = 0;
  uint16_t cmd = get_full_addressed_transaction_header(transaction_read, SYS_STATUS_BASE_REGISTER, SYS_STATUS_SUB_ADDRESS);
  SPI.transfer16(cmd);

  for (size_t i = 0; i < SYS_STATUS_LENGTH; i++) {
    status |= SPI.transfer16(cmd) << i * 8;
  }

  digitalWrite(SPI_chipselect, HIGH);
  SPI.endTransaction();

  return status;
}

void DW3000_Interface::configure_message_reception() {
  execute_nonreturning_SPI_command(get_fast_command_transaction_header(CMD_RX));
}

void DW3000_Interface::configure_message_transmission() {
  // preamble length, frame length, data rate und PRF in TX_FCTRL und CHAN_CTRL setzen
  // mit masked write transaction!
}

TRIA_Packet DW3000_Interface::receive_packet() {
  // warten bis Paket ankommt
  while (!(get_sys_status() & RXFR)) { delayMicroseconds(100); }
  // 2. angekommene Daten aus RX_BUFFER_0 auslesen und in Paket schreiben
  // 2. RX_STAMP auslesen und in Paket schreiben
  // 3. Paket zurückgeben
}

void DW3000_Interface::send_packet(TRIA_Packet& packet) {
  // warten bis letztes Paket gesendet wurde
  while (!(get_sys_status() & TXFRS)) { delayMicroseconds(100); }
  // 2. Paket in TX_BUFFER schreiben
  // TODO: nachsehen ob TX_STAMP automatisch geschrieben wird
  execute_nonreturning_SPI_command(CMD_TX);
}