/*
 * RGB_LED_driver.cpp
 *
 * Created: 8/24/2015 10:44:05 PM
 *  Author: Maxim
 */ 

// #define F_CPU 14745600
#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <modular8.h>
#include <s3p.h>

struct data
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} colors;

ISR(TIMER0_COMPA_vect) {
	
	// PORTB = ~PORTB;
	// s3p_transmit(tick_msg, sizeof(tick_msg));
}

ISR(TIMER1_COMPA_vect) {
	
	// static char tock_message[] = "eyyeah!";
	// s3p_transmit(tock_message, sizeof(tock_message));
}

int main(void) 
{
	s3p_init();
	
	TCCR0A = _BV(WGM01); // CTC mode
	TCCR0B =  PRESCALER_1024;
	OCR0A =	249;	
	//TIMSK0 = _BV(OCIE0A); // enable timer interrupt
	
	TCCR1A = 0;
	TCCR1B = _BV(WGM12) | PRESCALER_1024; // CTC mode
	OCR1AH = 0x20;
	OCR1AL = 0x00;
	TIMSK1 = _BV(OCIE1A); // enable timer interrupt
	
	colors.r = 255;
	colors.g = 160;
	colors.b = 32;
	
	sei(); // set interrupts
	
    while(1)	
	{
		// idle
	}
}