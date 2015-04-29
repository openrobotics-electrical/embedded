/*
 * arm_servo.cpp
 *
 * Created: 29/04/2015 14:06:58
 *  Author: Brad
 */ 


#define F_CPU					16000000L
#define CHAR_BUFFER_LENGTH		20
#define COM_SPEED				57600

#include <avr/io.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h> 
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdarg.h>
#include "../../lib/usart.cpp"
#include "../../lib/io_utils.h"

//------------------VARIABLES

char strOut[CHAR_BUFFER_LENGTH];
char strIn[CHAR_BUFFER_LENGTH];

//------------------MAIN

int main(void) {
	
	USART_Init(convertBaud(COM_SPEED));
	
	
		DDRB = 0b11110000;
		DDRC = 0b00000000;
		DDRD = 0b11111111;
		int i = 0;
		const int Period = 20;
		float angle1 = 1;
		float angle2 = 2;
		while(1){
			PORTD =  PORTD ^ 0x04;
			
			for(i=0;i<100;i++)
			{
				PORTD = PORTD ^ 0x08;
				_delay_ms(angle1);
				PORTD = PORTD & 0x04;
				_delay_ms(Period-angle1);
			}
			PORTD = PORTD & 0x08;
			
			for(i=0;i<100;i++)
			{
				
				
				PORTD = PORTD ^ 0x08;
				_delay_ms(angle2);
				PORTD = PORTD & 0x04;
				_delay_ms(Period-angle2);
			}
			

			
		}
}
