#include <lib/DW3000_interface.h>
#include <lib/DW3000_DECA_API/Src/decadriver/deca_regs.h>
#include <Arduino.h>

void DW3000_Interface::reset_DW3000() {
  pinMode(SPI_reset, OUTPUT);
  digitalWrite(SPI_reset, LOW);
  delayMicroseconds(100);
  pinMode(SPI_reset, INPUT);
  delayMicroseconds(1000);
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
  dwt_writefastCMD(CMD_TX);
}