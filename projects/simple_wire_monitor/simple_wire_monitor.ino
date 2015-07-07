#include <Wire.h>
#include <Arduino.h>

void setup() {
  Wire.onReceive(receiveEvent);
  Wire.begin(0);
  Serial.begin(115200);
}

void receiveEvent(int args) {
  while(Wire.available() > 0)
    Serial.write(Wire.read());
}

void loop() {
  Serial.println("...");
  delay(1000);
}
