#include <src/ComponentBridge.h>

ComponentBridge bridge;

void dw_receive_interrupt_handler(const dwt_cb_data_t *cb_data) {
  bridge.receive_dw_message(cb_data);
}

void setup() { bridge.initialise(dw_receive_interrupt_handler); }

void loop() {
  if (bridge.did_receive_dw_messages()) {
    BENCHMARK(bridge.handle_received_dw_messages());
  }

  if (bridge.did_receive_lora_messages()) {
    BENCHMARK(bridge.handle_received_lora_messages());
  }

  if (bridge.current_measurement_finished()) {
    BENCHMARK(bridge.send_current_measurement());
  }

  if (bridge.measurement_requested()) {
    BENCHMARK(bridge.start_measurement());
  }
}