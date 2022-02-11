#pragma once

#include <packets/TRIA_MeasureReport.h>

class USB_Interface {
public:
  static const uint8_t MEASURE_COMMAND = 0x6D; // m

  USB_Interface() {};

  bool measurement_requested();
  void send_measurement(TRIA_MeasureReport &report);

private:
  uint8_t m_data[TRIA_MeasureReport::PACKED_SIZE];
};