// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup() {
  
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
  // Wire.onReceive(receiveEvent);
  
  Serial.println("Master Controller");
  Serial.println("max@theprogrammingclub.com");
  Serial.println("Type ? for options");
}

volatile uint8_t dataBuffer[16];

void loop() {
  
  while(Serial.available()) {
    
    char in = Serial.read();  
    
    if(in == '?') {
      print_menu();
    } else if(in == 'l') {
      list_devices();
    } else if(in == 'q') {
      get_uint8_t(1);
    } else if(in == 'Q') {
      Serial.println(get_uint16_t(1));
    }
  } 
   /*
  Wire.requestFrom(1, 3);
  
  Serial.println("requesting 3 bytes from address 1");
  printReceived();
  
  Wire.beginTransmission(1);
  Wire.write(5);
  Wire.endTransmission();
  */
  //delay(1000);
}  

void print_menu() {
  
  Serial.println("[l]ist devices"); 
  Serial.println("[q]uery 8-bit");
  Serial.println("[Q]uery 16-bit");
}

void get_uint8_t(int address) {
  
  Wire.beginTransmission(address);
  Wire.write(1);
  Wire.endTransmission();
  
  Wire.requestFrom(address, 1);
  delayMicroseconds(15);
  
  if(Wire.available());
    Serial.println((int)Wire.read());
}

uint16_t get_uint16_t(uint8_t address) {
  
  Wire.beginTransmission(address);
  Wire.write(2);
  Wire.endTransmission();
  
  Wire.requestFrom(address, 2);
  delayMicroseconds(15);
 
  uint8_t high, low;
 
  if(Wire.available()) 
    low = Wire.read();
  if(Wire.available())
    high = Wire.read();
 
  return(high * 0x100 + low);
}

void list_devices() {
  
  volatile uint8_t bytes_returned;
  
  for(int i = 0; i < 128; i++) {
    
    bytes_returned = 0;
    
    Wire.beginTransmission(i);
    Wire.write(0);
    Wire.endTransmission();
    
    Wire.requestFrom(i, 16);
    
    delay(1);
    
    uint8_t j = 0;
    bytes_returned = getReceived(dataBuffer);
    
    if(bytes_returned > 0) {
      
      Serial.print(String(i) + " ");
        
      while(bytes_returned > j) {
        
        Serial.print((char)dataBuffer[j]);
        j++;
      }
      Serial.print('\n');
    }
  }
}

uint8_t getReceived(volatile uint8_t *receiveBuffer) {
  
  if(Wire.available()) {
      
    volatile uint8_t i = 0;
    
    do {
      receiveBuffer[i] = Wire.read();
      i++;
    } 
    while(0 < Wire.available());

    return i;
  }
  return 0;
}

void printReceived() {
	
  if(Wire.available()) {
    
    uint32_t input = 0;
    uint8_t i = 0;
  	
    do {
      
      input = (Wire.read() << i) + input;
      i += 8;
    
    } while(0 < Wire.available());
  
    Serial.println(input);
  }
}
