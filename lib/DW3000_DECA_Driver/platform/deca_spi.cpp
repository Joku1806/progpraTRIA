#include <SPI.h>
#include <decadriver/deca_regs.h>
#include <lib/assertions.h>
#include <platform/deca_spi.h>
#include <platform/pin_mappings.h>

SPISettings spi_settings;
void DWIC_set_spi_rate(size_t spi_rate) {
  spi_settings = SPISettings(spi_rate, MSBFIRST, SPI_MODE0);
}

void DWIC_reset() {
  pinMode(SPI_reset, OUTPUT);
  digitalWrite(SPI_reset, LOW);
  delayMicroseconds(100);
  pinMode(SPI_reset, INPUT);
  delayMicroseconds(1000);

  while (!dwt_checkidlerc()) {};
  DWIC_set_spi_rate(2000000);
  VERIFY(dwt_initialise(DWT_DW_INIT) == DWT_SUCCESS);
}

// FIXME: hat eigentlich nichts mit SPI zu tun
void DWIC_configure_spi(size_t spi_rate) {
  dwt_config_t config = {
      .chan = 5,
      .txPreambLength = DWT_PLEN_256,
      .rxPAC = DWT_PAC16,
      .txCode = 9,
      .rxCode = 9,
      .sfdType = DWT_SFD_DW_8,
      .dataRate = DWT_BR_850K,
      .phrMode = DWT_PHRMODE_STD,
      .phrRate = DWT_PHRRATE_STD,
      .sfdTO = 256 + 1 + 8 - 16, /* SFD timeout (preamble length + 1 + SFD length - PAC size) */
      .stsMode = DWT_STS_MODE_OFF,
      .stsLength = DWT_STS_LEN_64,
      .pdoaMode = DWT_PDOA_M0,
  };

  VERIFY(dwt_configure(&config) == DWT_SUCCESS);

  dwt_txconfig_t tx_config = {
      .PGdly = 0x34,
      .power = 0xfdfdfdfd,
      .PGcount = dwt_calcpgcount(0x34, 5),
  };
  dwt_configuretxrf(&tx_config);

  dwt_setrxantennadelay(16372);
  dwt_settxantennadelay(16405);

  DWIC_set_spi_rate(spi_rate);
}

void rx_timeout_handler(const dwt_cb_data_t *cb_data) { dwt_writefastCMD(CMD_RX); }

void rx_error_handler(const dwt_cb_data_t *cb_data) { dwt_writefastCMD(CMD_RX); }

void DWIC_configure_interrupts(void (*recv_handler)(const dwt_cb_data_t *cb_data)) {
  dwt_setcallbacks(NULL, recv_handler, rx_timeout_handler, rx_error_handler, NULL, NULL);
  dwt_setinterrupt(DWT_INT_RPHE | DWT_INT_RFCG | DWT_INT_RFCE | DWT_INT_RFSL | DWT_INT_RFTO |
                       DWT_INT_SFDT | DWT_INT_RXPTO,
                   0, DWT_ENABLE_INT);
  pinMode(SPI_interrupt, INPUT_PULLUP);
  digitalWrite(SPI_interrupt, LOW);
  attachInterrupt(digitalPinToInterrupt(SPI_interrupt), dwt_isr, RISING);

  // FIXME: wird das gebraucht?
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RCINIT_BIT_MASK | SYS_STATUS_SPIRDY_BIT_MASK);
}

int writetospiwithcrc(uint16_t headerLength, const uint8_t *headerBuffer, uint16_t bodyLength,
                      const uint8_t *bodyBuffer, uint8_t crc8) {
  SPI.beginTransaction(spi_settings);
  digitalWrite(SPI_chipselect, LOW);

  for (size_t i = 0; i < headerLength; i++) {
    SPI.transfer(headerBuffer[i]);
  }

  for (size_t i = 0; i < bodyLength; i++) {
    SPI.transfer(bodyBuffer[i]);
  }

  SPI.transfer(crc8);

  digitalWrite(SPI_chipselect, HIGH);
  SPI.endTransaction();

  return 0;
}

int writetospi(uint16_t headerLength, const uint8_t *headerBuffer, uint16_t bodyLength,
               const uint8_t *bodyBuffer) {
  SPI.beginTransaction(spi_settings);
  digitalWrite(SPI_chipselect, LOW);

  for (size_t i = 0; i < headerLength; i++) {
    SPI.transfer(headerBuffer[i]);
  }

  for (size_t i = 0; i < bodyLength; i++) {
    SPI.transfer(bodyBuffer[i]);
  }

  digitalWrite(SPI_chipselect, HIGH);
  SPI.endTransaction();

  return 0;
}

int readfromspi(uint16_t headerLength, uint8_t *headerBuffer, uint16_t readlength,
                uint8_t *readBuffer) {
  SPI.beginTransaction(spi_settings);
  digitalWrite(SPI_chipselect, LOW);

  for (size_t i = 0; i < headerLength; i++) {
    SPI.transfer(headerBuffer[i]);
  }

  for (size_t i = 0; i < readlength; i++) {
    readBuffer[i] = SPI.transfer(0x00);
  }

  digitalWrite(SPI_chipselect, HIGH);
  SPI.endTransaction();

  return 0;
}