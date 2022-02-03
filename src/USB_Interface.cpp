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

bool USB_Interface::schedule_full() { return m_index == MAX_ENTRIES; }

void USB_Interface::schedule_report(TRIA_RangeReport &r) {
  VERIFY(m_index < MAX_ENTRIES);

  r.pack_into(m_data + m_index * TRIA_RangeReport::PACKED_SIZE);
  m_index++;
}

void USB_Interface::schedule_reset() { m_index = 0; }

void USB_Interface::send_scheduled_reports() {
  Serial.write((uint8_t *)(&m_index), sizeof(m_index));
  Serial.write(m_data, m_index * TRIA_RangeReport::PACKED_SIZE);
  Serial.flush();
}
