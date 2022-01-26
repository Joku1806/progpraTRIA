#include <lib/assertions.h>
#include <src/DW3000_interface.h>
#include <src/USB_interface.h>

bool USB_Interface::measurement_requested() {
  if (!Serial.available())
    return false;

  uint8_t command;
  Serial.readBytes((char *)&command, 1);

  return command == MEASURE_COMMAND;
}

bool USB_Interface::schedule_full() { return m_index + 1 == MAX_ENTRIES; }

bool USB_Interface::schedule_likely_finished() {
  if (!m_last_insert_time_valid) {
    return false;
  }

  return micros() - MAX_DELAY_US >= m_last_insert_time;
}

void USB_Interface::schedule_report(TRIA_RangeReport &r) {
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
  // FIXME: wir hätten hier ein Problem, wenn m_index = UINT_MAX,
  // aber das kommt hoffentlich nirgendwann vor :^)
  unsigned measure_count = m_index + 1;
  Serial.write((uint8_t *)(&measure_count), sizeof(measure_count));
  Serial.write(m_data, measure_count * TRIA_RangeReport::PACKED_SIZE);
}
