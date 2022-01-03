#include <SPI.h>
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

void DWIC_configure_spi(size_t spi_rate) {
  // LoRa Chipselect auf HIGH schalten, damit er nicht während der SPI Kommunikation mit dem DW3000 stört.
  pinMode(LoRa_chipselect, OUTPUT);
  digitalWrite(LoRa_chipselect, HIGH);

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
  DWIC_set_spi_rate(spi_rate);
}

void DWIC_configure_interrupts(void (*recv_callback)(const dwt_cb_data_t *cb_data)) {
  dwt_setcallbacks(nullptr, recv_callback, nullptr, nullptr, nullptr, nullptr);
  // TODO: schauen ob dieser Pin Mode richtig ist
  pinMode(SPI_interrupt, INPUT_PULLUP);
  attachInterrupt(SPI_interrupt, dwt_isr, HIGH);

  // sofort Verbindungen annehmen
  dwt_writefastCMD(CMD_RX);
}

int writetospiwithcrc(uint16_t headerLength, const uint8_t *headerBuffer, uint16_t bodyLength, const uint8_t *bodyBuffer, uint8_t crc8) {
  decaIrqStatus_t stat = decamutexon();

  SPI.beginTransaction(spi_settings);

  for (size_t i = 0; i < headerLength; i++) {
    SPI.transfer(headerBuffer[i]);
  }

  for (size_t i = 0; i < bodyLength; i++) {
    SPI.transfer(bodyBuffer[i]);
  }

  SPI.transfer(crc8);

  SPI.endTransaction();
  decamutexoff(stat);

  return 0;
}

int writetospi(uint16_t headerLength, const uint8_t *headerBuffer, uint16_t bodyLength, const uint8_t *bodyBuffer) {
  decaIrqStatus_t stat = decamutexon();

  SPI.beginTransaction(spi_settings);

  for (size_t i = 0; i < headerLength; i++) {
    SPI.transfer(headerBuffer[i]);
  }

  for (size_t i = 0; i < bodyLength; i++) {
    SPI.transfer(bodyBuffer[i]);
  }

  SPI.endTransaction();
  decamutexoff(stat);

  return 0;
}

int readfromspi(uint16_t headerLength, uint8_t *headerBuffer, uint16_t readlength, uint8_t *readBuffer) {
  decaIrqStatus_t stat = decamutexon();

  SPI.beginTransaction(spi_settings);

  for (size_t i = 0; i < headerLength; i++) {
    SPI.transfer(headerBuffer[i]);
  }

  // FIXME: vllt noch ein extra Transfer davor?
  for (size_t i = 0; i < readlength; i++) {
    readBuffer[i] = SPI.transfer(0x00);
  }

  SPI.endTransaction();
  decamutexoff(stat);

  return 0;
}