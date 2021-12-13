#include <Arduino.h>
#include <lib/TRIA_Protocol/packets/TRIA_RangeRequest.h>

void TRIA_RangeRequest::print() {
  Serial.printf("Range Request von Unit %u an Unit(s) %u\n", received_from().id(), sent_to().id());
}