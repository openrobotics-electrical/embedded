#define DEVICE_NUMBER 2
#define DEVICE_NAME bjkb 

#include <Wire.h>
#include "communication_protocol.h"
#include "attribute.h"
#include <math.h>

typedef struct wireData {
  
  bool hasNewData;
  uint8_t request;
  char name16[16];
  
} WireData;

WireData wData = { false, 0x55 };
char request = 0;
bool newData = false;

Attribute x(1, "x axis", UINT32_T); 
Attribute y(2, "y axis", UINT32_T);
Attribute z(3, "z axis", UINT32_T);

void setup() {
  
  DDRB = DDRB | 0x20;
  pinMode(13, OUTPUT);
  pinMode(analogInputToDigitalPin(3), INPUT);

  sprintf(wData.name16, "ultrasounds      ");
  
  Wire.begin(DEVICE_NUMBER);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  
  Serial.begin(115200);
  Wire.begin();
  
  add(&x);
  add(&y);
  add(&z);
}
 
void requestEvent() {

  if(request == 0) {
  
    Wire.write(wData.name16, 16);
  } 
  else 
  if(request == 1) {
   
    Wire.write(101);
  } 
  else 
  if(request == 2) {
    
    uint8_t outBuffer[2];
    outBuffer[0] = 0x55;
    outBuffer[1] = 0x55;
    Wire.write(outBuffer, 2);
  }
}

void receiveEvent(int args) {
     
    request = Wire.read();
    newData = true;
}

int32_t count = 0;

void loop() {

	/*
	test.load((int32_t)analogRead(0));
	test2.load(0.5f);
	*/
	
	debug(String(count++));
	
	delayMicroseconds(100);
	analogRead(0);
	x.load((int32_t)analogRead(0) - 343);
	delayMicroseconds(100);
	poll();
	delayMicroseconds(100);
	analogRead(1);
	y.load((int32_t)analogRead(1) - 339);
	delayMicroseconds(100);
	poll();
	delayMicroseconds(100);
	analogRead(0);
	z.load((int32_t)analogRead(2) - 336);
	delayMicroseconds(100);
	poll();
}


