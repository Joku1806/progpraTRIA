#include <Arduino.h>
#include <SPI.h>
#include <stdio.h>

// GPIO pins für RSTn, SPICSn und IRQ
#define LoRa_CHIPSELECT 8
#define DWM3000_RESET 10
#define DWM3000_CHIPSELECT 11

#define DWM3000_RESET_WAIT_uS 1000
#define DWM3000_RESET_LOW_WAIT_uS 100
#define CMD_RESEND_WAIT 2000

void setup_DWM3000() {
  // damit der LoRa CS nicht stört
  pinMode(LoRa_CHIPSELECT, OUTPUT);
  digitalWrite(LoRa_CHIPSELECT, HIGH);

  pinMode(DWM3000_RESET, OUTPUT);
  digitalWrite(DWM3000_RESET, LOW);
  delayMicroseconds(DWM3000_RESET_LOW_WAIT_uS);
  pinMode(DWM3000_RESET, INPUT);
  delayMicroseconds(DWM3000_RESET_WAIT_uS);
}

uint8_t DW3000_get_device_id() {
  digitalWrite(DWM3000_CHIPSELECT, LOW);
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
  Serial.print("Device ID = ");
  Serial.println(dev_id);
  delay(CMD_RESEND_WAIT);
}