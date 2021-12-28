#ifndef PIN_MAPPINGS_H
#define PIN_MAPPINGS_H

enum m0_pin {
  LoRa_chipselect = 8,
  SPI_interrupt = 14, // A0, sollte vllt nochmal geändert werden weil das ein analoger Pin ist
  SPI_reset = 10,
  SPI_chipselect = 11,
};

#endif