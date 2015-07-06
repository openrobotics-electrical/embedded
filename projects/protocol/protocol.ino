#define DEVICE_NUMBER 2
#define DEVICE_NAME bjkb 

#include <Wire.h>
#include "attribute.h"

typedef enum STATUS { 

  WAITING_FOR_ADDRESS,
  SENDING,
  RECEIVING,
  IDLING 

} STATUS;

typedef struct wireData {
  
  bool hasNewData;
  uint8_t request;
  char name16[16];
  
} WireData;

WireData wData = { false, 0x55 };

void setup() {
  
  DDRB = DDRB | 0x20;
  pinMode(13, OUTPUT);

  sprintf(wData.name16, "ultrasounds      ");
  
  Wire.begin(DEVICE_NUMBER);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  
  Serial.begin(115200);
}
 
uint8_t request = '!';
boolean newData = false;

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

STATUS rx_status = IDLING, next_rx_status = IDLING;
STATUS tx_status = IDLING, next_tx_status = IDLING;

Attribute test(1, "TESTING", UINT32_T);
Attribute test2(2, "float test", FLOAT32_T);

Attribute getAttribute(int handle) {
	if(handle == 1) return test;
	else if(handle == 2) return test2;
}

void serialize(uint32_t u, byte *dataBuffer) {
	memcpy(dataBuffer, &u, sizeof u);
}

void serialize(float f, byte *dataBuffer) {
	memcpy(dataBuffer, &f, sizeof f);
}

char lastChar = '!';
uint32_t count = -200;
bool debug = false;
int index = 0;
int setAddress = 0;
int getAddress = 0;
uint8_t inputBuffer[BUFFER_LENGTH];

void flash() {
	PORTB = PINB ^ 0x20;
}

void loop() {

	if(Serial.available() > 0) {
	  	  	
		index %= BUFFER_LENGTH;
		lastChar = (char)Serial.read();
		serialize(count, test.data);
		serialize(32.5f / (count + 1), test2.data);

		if(tx_status == WAITING_FOR_ADDRESS) {
			
			digitalWrite(13, 1);
			count++;
			getAddress = (int)lastChar;
			while(!Serial) {};
			Serial.write(getAttribute(getAddress).data, 4);
			next_tx_status = IDLING;
			digitalWrite(13, 0);
			
		} else {

			switch(rx_status) {
			
				case RECEIVING:
					getAttribute(setAddress).data[index++] = (byte)lastChar;
				
					if(index == getAttribute(setAddress).messageType) {
					
						index = 0;
						next_rx_status = IDLING;
						getAttribute(setAddress).newDataAvailable(true);
					}
					break;

				case WAITING_FOR_ADDRESS:
					setAddress = (int)lastChar;
					next_rx_status = RECEIVING;
					break;
			
				case IDLING:
					if(lastChar == 'g') {
						next_tx_status = WAITING_FOR_ADDRESS;
					} else if(lastChar = 's') {
						next_rx_status = WAITING_FOR_ADDRESS;
					}
					break;
			}
		}
		
		tx_status = next_tx_status;
		rx_status = next_rx_status;
	}
}


