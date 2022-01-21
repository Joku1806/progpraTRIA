#include <src/DW3000_interface.h>
#include <src/USB_interface.h>
#include <lib/assertions.h>

bool USB_Interface::measurement_requested() {
  if (!Serial.available()) return false;
  
  uint8_t command;
  Serial.readBytes((char*)&command, 1);

  return command == MEASURE_COMMAND;
}

bool USB_Interface::schedule_full() {
  return m_index + 1 == MAX_ENTRIES;
}

bool USB_Interface::schedule_likely_finished() {
  // TODO: Formel für den maximalen Unterschied zwischen Ankunft von aufeinander folgenden Messungen finden
  return false;
}

void USB_Interface::schedule_report(TRIA_RangeReport& r) {
  VERIFY(m_index < MAX_ENTRIES);

  m_last_insert_time = micros();
  m_last_insert_time_valid = true;

  r.pack_into(m_data + m_index * TRIA_RangeReport::PACKED_SIZE);
  m_index++;
}

void USB_Interface::schedule_reset() {
  m_index = 0;
  m_last_insert_time_valid = false;
}

void USB_Interface::send_scheduled_reports() {
  Serial.write(m_index + 1);
  Serial.write(m_data, (m_index + 1) * TRIA_RangeReport::PACKED_SIZE);
}
