#include <src/DW3000_interface.h>

class USB_Interface {
  public:
    static const unsigned MAX_ENTRIES = 3;
    static const uint8_t MEASURE_COMMAND = 0xff;

    USB_Interface() {};
    
    bool measurement_requested();

    bool schedule_full();
    bool schedule_likely_finished();
    void schedule_report(TRIA_RangeReport& r);
    void send_scheduled_reports();
    void schedule_reset();

  private:
    bool m_last_insert_time_valid;
    unsigned m_last_insert_time;
    unsigned m_index;
    uint8_t m_data[MAX_ENTRIES * TRIA_RangeReport::PACKED_SIZE];
};

int get_repnum();
void scedule_report(TRIA_RangeReport rep);
int send_sceduled_reports();
void send_single_report(TRIA_RangeReport rep);