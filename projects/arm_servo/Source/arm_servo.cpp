/*
 * arm_servo.cpp
 *
 * Created: 29/04/2015 14:06:58
 *  Author: Brad
 *	This file contains cpp code for the Torxis Servo in the shoulder of the open robotics 
 * 
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
		sei();
		
		uint16_t i = 0;
		uint16_t j = 0; 
		const int Period = 200;
		uint16_t angle = 10;
		uint16_t adc_value = 0;
		
		//const int Period = 20;
		float angle1 = 1;
		float angle2 = 2;
		
		char str[20]; 
		Analog::selectChannel(0);
		
		while(1){
			
				/*
			
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
					
*/
					
				
			
			//PORTD =  PORTD ^ 0x04;
			
			Analog::startConversion();
			adc_value =Analog::getValue();
			angle = (adc_value*30)/1024;
		
			for (j = 0; j < 100; j++)
			{
		
				PORTD = PORTD ^ 0x08;
				for (i = 0; i < angle; i++) _delay_us(100);
				PORTD = PORTD & 0x00;
				for (i = 0; i < Period-angle; i++) _delay_us(100);	
					
			}
			
		/* Serial Debugging	
		sprintf(str, "A%u AN%u\n", angle, adc_value);
		USART_Send_string(str);
		*/			
	}
}
