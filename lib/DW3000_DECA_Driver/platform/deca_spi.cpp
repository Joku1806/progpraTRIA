#include <SPI.h>
#include <decadriver/deca_device_api.h>

SPISettings settings = SPISettings(10000000, MSBFIRST, SPI_MODE0);

int writetospiwithcrc(uint16_t headerLength, const uint8_t *headerBuffer, uint16_t bodyLength, const uint8_t *bodyBuffer, uint8_t crc8) {
  decaIrqStatus_t stat = decamutexon();

  SPI.beginTransaction(settings);

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

  SPI.beginTransaction(settings);

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

  SPI.beginTransaction(settings);

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