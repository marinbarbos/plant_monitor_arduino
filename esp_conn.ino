#include "SoftwareSerial.h"

SoftwareSerial ESP_Serial(2, 3);  // RX, TX

void setup() {
  Serial.begin(115200);
  ESP_Serial.begin(115200);
}

void loop() {
  if (ESP_Serial.available()) {
    Serial.write(ESP_Serial.read());
  }
  if (Serial.available()) {
    ESP_Serial.write(Serial.read());
  }
}