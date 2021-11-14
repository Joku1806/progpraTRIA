#include <Arduino.h>
#include <SPI.h>
#include <stdio.h>

// GPIO pins für RSTn, SPICSn und IRQ
#define DWM3000_RESET 10
#define DWM3000_CHIPSELECT 11
#define DWM3000_IRQ 12

#define DWM3000_RESET_WAIT 5
#define CMD_RESEND_WAIT 2000

void setup_DWM3000() {
  pinMode(DWM3000_IRQ, INPUT_PULLDOWN);
  pinMode(DWM3000_CHIPSELECT, OUTPUT);
  pinMode(DWM3000_RESET, OUTPUT);
  digitalWrite(DWM3000_RESET, LOW);
  pinMode(DWM3000_RESET, INPUT);
  delay(DWM3000_RESET_WAIT);
}

uint8_t DW3000_get_device_id() {
  digitalWrite(DWM3000_CHIPSELECT, LOW);
  delay(5);
  SPI.transfer(0);
  uint8_t id = SPI.transfer(0);
  digitalWrite(DWM3000_CHIPSELECT, HIGH);
  return id;
}

void setup() {
  setup_DWM3000();
  SPI.begin();
}

void loop() {
  uint8_t dev_id = DW3000_get_device_id();
  printf("Device ID = %u", dev_id);
  delay(CMD_RESEND_WAIT);
}