/*
 * ultrasound.cpp
 *
 * Created: 4/28/2015 9:28:17 AM
 *  Author: Max Prokopenko
 *  max@theprogrammingclub.com
 */

#include "../../lib/F_CPU.h"				// clock speed
#define CHAR_BUFFER_LENGTH		20
#define COM_SPEED				57600
#define ADC10BIT				((analogHigh << 2) | (analogLow >> 6))

#include <avr/io.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h> 
#include <avr/interrupt.h>
#include <stdlib.h>
#include "../../lib/usart.cpp"
#include "../../lib/io_utils.h"

//------------------PROTOTYPES

void softSerial(uint8_t message);
void startADC();
void selectADC(uint8_t n);

//------------------VARIABLES

volatile uint8_t analogLow, analogHigh;
char strOut[CHAR_BUFFER_LENGTH]; // debug output buffer
char strIn[CHAR_BUFFER_LENGTH]; // debug input buffer

//------------------INTERRUPT SERVICE ROUTINES

ISR(TIMER1_COMPA_vect) {
	
	TCNT1 = 0;
}

ISR(ADC_vect) {
	
	analogLow = ADCL;
	analogHigh = ADCH;
}

//------------------MAIN

int main(void) {
	
	DDRB = 0b11110000;
	DDRC = 0b00000000;
	DDRD = 0b11111111;
	
	USART_Init(convertBaud(COM_SPEED));
	
	while(1) {
		uint16_t uCount = 0;
		DDRD = 0b11111111;
		setBitTo(3, 0, &PORTD);
		setBitTo(2, 0, &PORTD);
		_delay_us(20);
		setBitTo(2, 1, &PORTD);
		_delay_us(20);
		setBitTo(3, 1, &PORTD);
		setBitTo(2, 0, &PORTD);
		DDRD = 0b11110111;
		while(!(PIND & 0b00001000));
		while(PIND & 0b00001000) {
			_delay_us(1);
			uCount++;
		}
		sprintf(strOut, "%u mm\n", (uint16_t)(uCount * 0.25));
		USART_Send_string(strOut);
		_delay_ms(50);
	}
}

//------------------FUNCTIONS

void selectADC(uint8_t n) {
	
	ADMUX  = 0b01100000 | n; // port n ADC selected
	ADCSRA = 0b10001011; // on, 2x clock
	ADCSRB = 0b00000000; // free running
}

void startADC() { setBitTo(ADSC, 1, &ADCSRA); }