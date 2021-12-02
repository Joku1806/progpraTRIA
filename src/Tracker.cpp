#include <lib/DW3000_interface.h>

DW3000_Interface interface;
TRIA_ID id;

static void receive_handler(const dwt_cb_data_t *cb_data) {
  std::optional<TRIA_Packet> ret = interface.handle_incoming_packet(cb_data->datalength);
  if (!ret) {
    return;
  }

  TRIA_Packet range_data = ret.value();
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
  // FIXME: Noch kein funktionierender constructor
  TRIA_Packet t = TRIA_Packet(TRIA_Action(range_request), id, TRIA_ID(tracker, 0));
  interface.send_packet(t);
  delay(2000);
  #endif
}