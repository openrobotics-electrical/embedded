/*
 * ultrasound.cpp
 *
 * Created: 4/28/2015 9:28:17 AM
 *  Author: Max Prokopenko
 *  max@theprogrammingclub.com
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

//------------------INTERRUPT SERVICE ROUTINES

ISR(TIMER1_COMPA_vect) {
	
	TCNT1 = 0;
}

//------------------MAIN

int main(void) {
	
	USART_Init(convertBaud(COM_SPEED));
	
	Bit trig(2, PORTD);
	Bit echo(3, PORTD);
	
	trig.setAsOutput();
	echo.setAsHighZInput();
	
	while(1) {
		
		uint16_t uCount = 0;
		
		trig.clear();
		_delay_us(20);	
		trig.set();
		_delay_us(20);
		trig.clear();
		
		while(echo.isLow()) {};
			
		while(echo.isHigh()) {
			
			_delay_us(1);
			uCount++;
		}
		
		sprintf(strOut, "%u mm\n", (uint16_t)(uCount * 0.25));
		USART_Send_string(strOut);
		_delay_ms(50);
	}
}



