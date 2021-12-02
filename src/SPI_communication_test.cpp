#include <Arduino.h>
#include <SPI.h>

// GPIO pins für RSTn und SPICSn
#define DWM3000_RESET 10
#define DWM3000_CHIPSELECT 11

SPISettings DWM3000_SPI_SETTINGS = SPISettings(10000000, MSBFIRST, SPI_MODE0);

#define SERIAL_CONNECT_WAIT 5000
#define CMD_RESEND_WAIT 2000
#define LORA_CS 8

uint8_t dev_id[4];
uint8_t dev_id2[8] = {0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88};

void reset_DWM3000() {
    pinMode(DWM3000_RESET, OUTPUT);
    digitalWrite(DWM3000_RESET, LOW);
    delayMicroseconds(100);
    pinMode(DWM3000_RESET, INPUT);
    delay(1); //give the DWM3000 chip time to come up
}

void get_device_id(uint8_t dev_id[]) {
    SPI.beginTransaction(DWM3000_SPI_SETTINGS);
    digitalWrite(DWM3000_CHIPSELECT, LOW);

    // read device id
    SPI.transfer(0x00);
    for(int i = 0; i < 4; i++) {
        dev_id[i] = SPI.transfer(0x00);
    }

    digitalWrite(DWM3000_CHIPSELECT, HIGH);
    SPI.endTransaction();
}

void setup() {
    delay(SERIAL_CONNECT_WAIT);
    reset_DWM3000();
    pinMode(DWM3000_CHIPSELECT, OUTPUT);

    pinMode(LORA_CS, OUTPUT);
    digitalWrite(LORA_CS, HIGH);

    SPI.begin();
    Serial.begin(9600);
}

void loop() {
    while (!Serial.available()) {}
    uint8_t command = Serial.read();
    Serial.print("Got command: 0x");
    Serial.print(command, 16);
    Serial.print("\n");
    get_device_id(dev_id);

    if (command == 0xff) {
      Serial.write(dev_id, 4);
      Serial.write(dev_id, 4);
      Serial.write(dev_id2, 8);
      Serial.write(125);
      Serial.write(126);
    }

    delay(CMD_RESEND_WAIT);
}
