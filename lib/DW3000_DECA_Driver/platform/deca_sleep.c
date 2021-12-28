#include <Arduino.h>

void deca_sleep(unsigned int time_ms) {
  delay(time_ms);
}

void deca_usleep(unsigned long time_us) {
  delayMicroseconds(time_us);
}
