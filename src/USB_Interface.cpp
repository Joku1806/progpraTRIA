#include <lib/assertions.h>
#include <src/DW3000_interface.h>
#include <src/USB_interface.h>

bool USB_Interface::measurement_requested() {
  if (!Serial.available()) {
    return false;
  }

  uint8_t command;
  Serial.readBytes((char *)&command, 1);

  return command == MEASURE_COMMAND;
}

bool USB_Interface::schedule_is_reset() { return !m_last_insert_time_valid && m_index == 0; }

bool USB_Interface::schedule_full() { return m_index == MAX_ENTRIES; }

bool USB_Interface::schedule_likely_finished() {
  if (!m_last_insert_time_valid) {
    return false;
  }

  unsigned long current_time = micros();
  return current_time < m_last_insert_time
             ? (ULONG_MAX - m_last_insert_time + current_time >= MAX_DELAY_US)
             : (current_time - m_last_insert_time >= MAX_DELAY_US);
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
  Serial.write((uint8_t *)(&m_index), sizeof(m_index));
  Serial.write(m_data, m_index * TRIA_RangeReport::PACKED_SIZE);
  Serial.flush();
}
