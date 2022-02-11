#include <src/ComponentBridge.h>

ComponentBridge bridge;

void dw_receive_interrupt_handler(const dwt_cb_data_t *cb_data) {
  BENCHMARK(bridge.receive_dw_message(cb_data););
}

void setup() { bridge.initialise(dw_receive_interrupt_handler); }

void loop() { bridge.execute(); }