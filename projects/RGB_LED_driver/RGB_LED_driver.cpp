/*
 * RGB_LED_driver.cpp
 *
 * Created: 8/24/2015 10:44:05 PM
 *  Author: Maxim
 */ 

// #define F_CPU 14745600
#define F_CPU 16000000

#define PRESCALER_1		0b001;
#define PRESCALER_8		0b010;
#define PRESCALER_64	0b011;
#define PRESCALER_256	0b100;
#define PRESCALER_1024	0b101;

#ifndef _BV
#define _BV(x) (1 << (x)) 
#endif
#define BITSET(x, y) ((x) |= (_BV(y)))
#define BITCLR(x, y) ((x) &= ~(_BV(y)))

#define FAKEBAUD 31

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <s3p.h>

struct data
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} colors;
	
uint8_t PWM_count;

uint8_t test = 'U';
uint8_t current_bit = 1;

void soft_transmit(char* s, uint8_t char_count) {
	
	// TIMSK0 = _BV(OCIE1A); // start timer0
	// PORTD = 0;
}

char tick_msg[] = "@RGB";
char tock_msg[] = "TOCK";

ISR(TIMER0_COMPA_vect) {
	
	PORTB ^= 2;
	s3p_transmit(tick_msg, sizeof(tick_msg));
}

int main(void) 
{
	s3p_init();

	DDRB = 0xff;
	DDRD = 0xff; // PORTD as outputs
	
	TCCR0A = _BV(WGM01); // CTC mode
	TCCR0B =  PRESCALER_64;
	OCR0A =	49;
	TIMSK0 = _BV(OCIE1A); // enable timer interrupt
	
	s3p_send_input_to(&colors, sizeof(colors));
	
	colors.r = 255;
	colors.g = 160;
	colors.b = 32;
	
	sei(); // set interrupts
	
	BITSET(PORTD, 6);
	
    while(1)	
	{
		PORTD = PORTD + 1;
		_delay_ms(50);
	}
}