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

#define COUNTS_PER_REVOLUTION 663
#define TIMER_COUNT 93 // F_CPU / (2 * 256 * COUNTS_PER_REVOLUTION) - 1
#define MOTOR_PIN _BV(PORTD3)

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

ISR(TIMER0_COMPA_vect) {

	PWM_count++;
	uint8_t output = 0;
	
	if(colors.r > PWM_count) BITSET(output, 3);
		else BITCLR(output, 3);
	if(colors.b > PWM_count) BITSET(output, 4);
		else BITCLR(output, 4);
	if(colors.g > PWM_count) BITSET(output, 2);
		else BITCLR(output, 2);

	PORTD = output;
}

int main(void) {

	s3p_init();

	DDRB = 0xff;
	DDRD = 0xff; // PORTD as outputs
	
	TCCR0A = (1 << WGM01); // CTC mode
	TCCR0B =  PRESCALER_8; // PRESCALER_8 default;
	OCR0A = TIMER_COUNT; // calculated above to give 4/663 of a second
	// TIMSK0 = _BV(OCIE1A); // enable timer interrupt
	
	s3p_send_input_to(&colors, sizeof(colors));
	
	colors.r = 255;
	colors.g = 160;
	colors.b = 32;
	
	sei(); // set interrupts
	
    while(1) {
	/*
		if(chars_unread > 3) {
			
			char message[] = "got em";
			serial_transmit(message, 6);
			chars_unread = 0;
		}
		*/
		char tick_msg[] = "TICK\n";
		char tock_msg[] = "TOCK\n";
		_delay_ms(500);
		s3p_transmit(tick_msg, sizeof(tick_msg));
		_delay_ms(500);
		s3p_transmit(tock_msg, sizeof(tock_msg));
	}
}