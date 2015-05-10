/*
 * arm_control.cpp
 *
 * Created: 4/29/2015 5:09:36 PM
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
#include "../../lib/SoftwareSerial.h"
#include "../../lib/DynamixelSoftSerial/DynamixelSoftSerial.cpp"

//------------------VARIABLES

char strOut[CHAR_BUFFER_LENGTH];
char strIn[CHAR_BUFFER_LENGTH];

//------------------MAIN

int main(void) {
	
	USART_Init(convertBaud(COM_SPEED));
	
	Bit	d1G_(2, PORTD);
	
	Bit d1A1();
	Bit d1A2();
	Bit d1A3();	
	Bit d1A4();
	
	Bit d1Y1();
	Bit d1Y2();
	Bit d1Y3();
	Bit d1Y4();
	
	Bit d2G_(3, PORTD);
	
	Bit d2A1();
	Bit d2A2();
	Bit d2A3();
	Bit d2A4();
	
	Bit d2Y1();
	Bit d2Y2();
	Bit d2Y3();
	Bit d2Y4();
	
	SoftwareSerial s(4, 5, PORTD);
	
	while(1) {
		
		s.setMode(1);
		s.write(1);
		_delay_ms(200);
		// s.setMode(0);
		s.write(0);
		_delay_ms(200);
	}
}
