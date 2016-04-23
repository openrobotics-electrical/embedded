/*
 * slowscilloscope.cpp
 *
 * Created: 8/24/2015 10:44:05 PM
 *  Author: Maxim
 */ 

#define F_CPU 16000000
#define BAUD 57600
#define BAUDRATE_DIVISOR F_CPU/8/BAUD - 1
#define NUMBER_CHANNELS 8

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <modular8.h>
#include <S3P.h>
#include <Analog.h>
#include "dataStructures.h"
#include <stdio.h>
#include <string.h>

int main(void) 
{
	S3P::init(BAUDRATE_DIVISOR, DATA_STRUCTURE_REF);
	Analog::selectChannel(0);
	Analog::autoTriggerEnable(false);
	Analog::startConversion();
	sei(); // set interrupts
	
	DDRC = 0x00;
	
	ATOMIC(dataOut.test = 0);
	
	uint16_t v_arr[10][NUMBER_CHANNELS];
	uint16_t v_results[NUMBER_CHANNELS];
	
    while(1)	
	{
		for(int i = 0; i < 10; i++) {
			for(int j = 0; j < NUMBER_CHANNELS; j++) {
				Analog::selectChannel(j);
				Analog::startConversion();
				while(!Analog::newValueAvailable()) 
				{ 
					/*do nothing*/
				}
				v_arr[i][j] = (uint16_t)Analog::getValue() * 4.882813;
			}
		}
		for(int j = 0; j < NUMBER_CHANNELS; j++) {
			v_results[j] = 0;
			for(int i = 0; i < 10; i++) {
				v_results[j] += v_arr[i][j]; 
			}
			ATOMIC(dataOut.voltage[j] = (uint16_t)v_results[j] / 10);
		}
		/*
		char message[80];
		sprintf(message, "%1d.%03d%2d.%03d%2d.%03d%2d.%03d%2d.%03d%2d.%03d%2d.%03d%2d.%03d\n",
				dataOut.voltage[0] / 1000, dataOut.voltage[0] % 1000,
				dataOut.voltage[1] / 1000, dataOut.voltage[1] % 1000,
				dataOut.voltage[2] / 1000, dataOut.voltage[2] % 1000,
				dataOut.voltage[3] / 1000, dataOut.voltage[3] % 1000,
				dataOut.voltage[4] / 1000, dataOut.voltage[4] % 1000,
				dataOut.voltage[5] / 1000, dataOut.voltage[5] % 1000,
				dataOut.voltage[6] / 1000, dataOut.voltage[6] % 1000,
				dataOut.voltage[7] / 1000, dataOut.voltage[7] % 1000);
				*/
		//S3P::transmit(&message, strlen(message));
		_delay_ms(100);
	}
}