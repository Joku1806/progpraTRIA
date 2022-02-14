#include <Arduino.h>
#include <src/USB_interface.h>

bool USB_Interface::measurement_requested() {
  if (!Serial.available()) {
    return false;
  }

  uint8_t command;
  Serial.readBytes((char *)&command, 1);

  return command == MEASURE_COMMAND;
}

void USB_Interface::send_measurement(TRIA_MeasureReport &report) {
#ifdef DEBUG
  Serial.println("Sending report to data team.");
  report.print();
  Serial.print("\n");
#endif

  unsigned fragment_count = report.entries();
  report.pack_into(m_data);

  Serial.write((uint8_t *)(&fragment_count), sizeof(fragment_count));
  Serial.write(m_data + 3, report.packed_size() - 3);
  Serial.flush();
}