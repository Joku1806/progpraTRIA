#include <src/DW3000_interface.h>

class USB_Interface {
public:
  static const unsigned MAX_ENTRIES = 3;
  static const uint8_t MEASURE_COMMAND = 0x6D; // m

  static const unsigned MAX_DELAY_US = 14; // 4000m / c_air von s => us

  USB_Interface() : m_last_insert_time_valid(false), m_last_insert_time(0), m_index(0) {};

  bool measurement_requested();

  bool schedule_full();
  bool schedule_likely_finished();
  void schedule_report(TRIA_RangeReport &r);
  void send_scheduled_reports();
  void schedule_reset();

private:
  volatile bool m_last_insert_time_valid;
  volatile unsigned m_last_insert_time;
  volatile unsigned m_index;
  uint8_t m_data[MAX_ENTRIES * TRIA_RangeReport::PACKED_SIZE];
};

int get_repnum();
void scedule_report(TRIA_RangeReport rep);
int send_sceduled_reports();
void send_single_report(TRIA_RangeReport rep);