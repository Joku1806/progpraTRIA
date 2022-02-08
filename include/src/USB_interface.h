#include <src/DW3000_interface.h>

class USB_Interface {
public:
  static const unsigned MAX_ENTRIES = 3;
  static const uint8_t MEASURE_COMMAND = 0x6D; // m

  USB_Interface() {};

  bool measurement_requested();

  bool schedule_full();
  void schedule_report(TRIA_RangeReport &r);
  void send_scheduled_reports();
  void schedule_reset();

private:
  unsigned m_index {0};
  uint8_t m_data[MAX_ENTRIES * TRIA_RangeReport::PACKED_SIZE];
};