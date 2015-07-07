#ifndef COMMUNICATION_PROTOCOL_H
#define COMMUNICATION_PROTOCOL_H

#include <Wire.h>
#include "attribute.h"

#define MAX_ATTRIBUTES 128

//--------------------------TYPEDEFS

typedef enum STATUS {

	WAITING_FOR_ADDRESS,
	SENDING,
	RECEIVING,
	IDLING

} STATUS;

//--------------------------GLOBAL VARIABLES

Attribute *attributes[MAX_ATTRIBUTES];
STATUS rx_status = IDLING, next_rx_status = IDLING;
STATUS tx_status = IDLING, next_tx_status = IDLING;
char lastChar = '!';
int index = 0;
int setAddress = 0, getAddress = 0;
uint8_t inputBuffer[BUFFER_LENGTH];

//--------------------------GLOBAL FUNCTIONS

void add(Attribute *a) {
	attributes[a->reference] = a;
}

void debug(String s) {
	Wire.beginTransmission(0);
	Wire.println(s);
	Wire.endTransmission();
}

void poll() {
	
	while(Serial.available() > 0) {
		
		index %= BUFFER_LENGTH;
		lastChar = (char)Serial.read();
		
		if(tx_status == WAITING_FOR_ADDRESS) {
			
			getAddress = (int)lastChar;
			while(!Serial) {};
			Serial.write(attributes[getAddress]->data, 4);
			next_tx_status = IDLING;
			
		} else {

			switch(rx_status) {
				
				case RECEIVING:
					attributes[setAddress]->data[index++] = (byte)lastChar;
				
					if(index == attributes[setAddress]->messageType) {
					
						index = 0;
						next_rx_status = IDLING;
						attributes[setAddress]->newDataAvailable(true);
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

#endif