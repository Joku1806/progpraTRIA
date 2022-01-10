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

  DWIC_set_spi_rate(2000000);
  VERIFY(dwt_initialise(DWT_DW_INIT) == DWT_SUCCESS);
}

// FIXME: hat eigentlich nichts mit SPI zu tun
void DWIC_configure_spi(size_t spi_rate) {
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

  VERIFY(dwt_configure(&config) == DWT_SUCCESS);
  DWIC_set_spi_rate(spi_rate);
}

volatile bool interrupts_work = false;

void test_interrupts(void) { interrupts_work = true; }

void DWIC_configure_interrupts(void (*tx_handler)(const dwt_cb_data_t *cb_data),
                               void (*recv_handler)(const dwt_cb_data_t *cb_data)) {
  dwt_setcallbacks(tx_handler, recv_handler, NULL, NULL, NULL, NULL);
  dwt_setinterrupt(SYS_ENABLE_LO_TXFRS_ENABLE_BIT_MASK | SYS_ENABLE_LO_RXFCG_ENABLE_BIT_MASK, 0,
                   DWT_ENABLE_INT);
  pinMode(SPI_interrupt, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SPI_interrupt), test_interrupts, RISING);
  digitalWrite(SPI_interrupt, LOW);
  delayMicroseconds(100);
  digitalWrite(SPI_interrupt, HIGH);

  if (interrupts_work) {
    Serial.println("attachInterrupt() funktioniert.");
  } else {
    Serial.println("attachInterrupt() funktioniert nicht.");
  }
  // sofort Verbindungen annehmen
  dwt_writefastCMD(CMD_RX);
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