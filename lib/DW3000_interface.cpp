#include <lib/DW3000_interface.h>
#include <lib/DW3000_DECA_API/Src/decadriver/deca_regs.h>
#include <Arduino.h>

DW3000_Interface::DW3000_Interface() {
  m_spi_settings = SPISettings(10000000, MSBFIRST, SPI_MODE0);
  reset_DW3000();
  dwt_initialise(DWT_DW_INIT);

  // TODO: nachfragen ob diese Werte in Ordnung sind
  dwt_config_t config = {
    .chan = 5,
    .txPreambLength = DWT_PLEN_32,
    .rxPAC = DWT_PAC4,
    .txCode = 3,
    .rxCode = 3,
    .sfdType = DWT_SFD_DW_16,
    .dataRate = DWT_BR_6M8,
    .phrMode = DWT_PHRMODE_STD,
    .phrRate = DWT_BR_6M8,
    .sfdTO = DWT_PLEN_32 + 1 + DWT_SFD_DW_16 - DWT_PAC4,
    .stsMode = DWT_STS_MODE_OFF,
    .stsLength = DWT_STS_LEN_32,
    .pdoaMode = DWT_PDOA_M0,
  };

  dwt_configure(&config);
  dwt_setcallbacks(nullptr, receive_packet, nullptr, nullptr, nullptr, nullptr);

  dwt_writefastCMD(CMD_RX);
}

void DW3000_Interface::reset_DW3000() {
  pinMode(SPI_reset, OUTPUT);
  digitalWrite(SPI_reset, LOW);
  delayMicroseconds(100);
  pinMode(SPI_reset, INPUT);
  delayMicroseconds(1000);
}

void DW3000_Interface::receive_packet(const dwt_cb_data_t *cb_data) {
  // 1. angekommene Daten aus RX_BUFFER_0 auslesen und in Paket schreiben
  // 2. falls Paket nicht für uns bestimmt, ignorieren (DW3000_Interface braucht ID!)
  // 3. falls range request:
  //    3.1. Typ zu range response ändern
  //    3.2. RX_STAMP in Paket schreiben
  //    3.3. Paket zurücksenden
  // 4. falls range response:
  //    4.1. Typ zu range report ändern
  //    4.2. RX_STAMP_packet = RX_STAMP - (TX_STAMP_packet - RX_STAMP_packet)
  //    4.3. TX_STAMP_packet = eigene gespeicherte TX_STAMP
  //    4.4. modifiziertes Paket in Queue schreiben
  //    4.5. dem Host signalisieren, dass ein neues Paket angekommen ist. Wie macht man das am besten?
}

void DW3000_Interface::send_packet(TRIA_Packet& packet) {
  // warten bis letztes Paket gesendet wurde
  while (!(get_sys_status() & TXFRS)) { delayMicroseconds(100); }
  // 2. Paket in TX_BUFFER schreiben
  // TODO: nachsehen ob TX_STAMP automatisch geschrieben wird
  dwt_writefastCMD(CMD_TX);
}