#include <Arduino.h>

#define SLEEP_AMOUNT_MS 200

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, INPUT_PULLDOWN);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
  delay(SLEEP_AMOUNT_MS);
  digitalWrite(LED_BUILTIN, LOW);
  delay(SLEEP_AMOUNT_MS);
}