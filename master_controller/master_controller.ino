// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup()
{
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
  Wire.onReceive(receiveEvent);
}

byte x = 0;

void loop()
{
  Wire.requestFrom(1, 3);
  
  Serial.println("requesting 3 bytes from address 1");
  printReceived();
  
  Wire.beginTransmission(1);
  Wire.write(5);
  Wire.endTransmission();

  delay(1000);
}  

void receiveEvent(int howMany) {
  
    Serial.println("received " + String(howMany) + "bytes");
    printReceived();
}

void printReceived() {
	
	uint32_t input = 0;
	uint8_t i = 0;
	
    while(0 < Wire.available()) {
		input = (Wire.read() << i) + input;
		i += 8;
    }
	Serial.println(input);
}
