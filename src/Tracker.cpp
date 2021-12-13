#include <src/DW3000_interface.h>
#include <src/Tracker.h>

#ifdef SENDER
TRIA_ID id = TRIA_ID(tracker, 1);
#else
TRIA_ID id = TRIA_ID(tracker, 2);
#endif
DW3000_Interface interface = DW3000_Interface(id, receive_handler);
TRIA_RangeReport report;

void receive_handler(const dwt_cb_data_t *cb_data) {
  auto got_report = interface.handle_incoming_packet(cb_data->datalength, report);
  if (!got_report) {
    return;
  }

  report.print();
}

void setup() {}

void loop() {
#ifdef SENDER
  auto t = TRIA_RangeRequest(id, TRIA_ID(tracker, 0));
  interface.send_packet(t);
  delay(2000);
#endif
}