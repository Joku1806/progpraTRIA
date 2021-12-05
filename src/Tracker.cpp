#include <src/DW3000_interface.h>

DW3000_Interface interface;
TRIA_ID id;

static void receive_handler(const dwt_cb_data_t *cb_data) {
  std::optional<TRIA_RangeReport> ret = interface.handle_incoming_packet(cb_data->datalength);
  if (!ret) {
    return;
  }

  TRIA_RangeReport range_data = ret.value();
  range_data.print();
}

void setup() {
  interface = DW3000_Interface(id, receive_handler);

#ifdef SENDER
  TRIA_ID id = TRIA_ID(tracker, 1);
#else
  TRIA_ID id = TRIA_ID(tracker, 2);
#endif
}

void loop() {
#ifdef SENDER
  auto t = TRIA_RangeRequest(id, TRIA_ID(tracker, 0));
  interface.send_packet(t);
  delay(2000);
#endif
}