#define DEVICE_NUMBER 2
#define DEVICE_NAME bjkb 

#include <Wire.h>

enum MESSAGE_TYPE { UINT32_T = 4, FLOAT32_T = 4, CHAR = 1 };

typedef struct wireData WireData;

struct wireData {
  
  bool hasNewData;
  uint8_t request;
  char name16[16];
};

WireData data = { false, 0x55 };

void setup() {
  
  DDRB = DDRB | 0x20;

  sprintf(data.name16, "ultrasounds      ");
  
  Wire.begin(DEVICE_NUMBER);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  
  Serial.begin(115200);
}
 
uint8_t request = '!';
boolean newData = false;

void requestEvent() {

  if(request == 0) {
  
    Wire.write(data.name16, 16);
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

void loop() {
  
  while(Serial.available() > 0) {
    char c = (char)Serial.read();
    if(c == 'g') {
      Serial.print("GET DETECTED\n");
    } else if(c == 's')
      Serial.print("SET DETECTED\n");
  }
  PORTB = PINB ^ 0x20;
}

