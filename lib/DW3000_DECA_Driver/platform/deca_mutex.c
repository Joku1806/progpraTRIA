#include <Arduino.h>
#include <decadriver/deca_device_api.h>
#include <platform/pin_mappings.h>

inline void disable_IRQ_interrupts() {
  asm volatile("cpsid i" ::
                   : "memory");
}

inline void enable_IRQ_interrupts() {
  asm volatile("cpsie i" ::
                   : "memory");
}

inline uint32_t get_interrupt_pin_status() {
  return digitalRead(SPI_interrupt);
}

decaIrqStatus_t decamutexon(void) {
  decaIrqStatus_t s = get_interrupt_pin_status();

  if (s) {
    disable_IRQ_interrupts();
  }

  return s;
}

void decamutexoff(decaIrqStatus_t s) {
  if (s) {
    enable_IRQ_interrupts();
  }
}
