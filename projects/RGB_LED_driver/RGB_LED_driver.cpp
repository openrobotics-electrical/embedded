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
#include <util/delay.h>
#include <modular8.h>
#include <s3p.h>
#include <analog.h>

volatile struct data_in
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} Data_in;

volatile struct data_out
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint16_t test;
} Data_out;

ISR(TIMER0_COMPA_vect) {
	
	// PORTB = ~PORTB;
	// s3p_transmit(tick_msg, sizeof(tick_msg));
}

ISR(TIMER1_COMPA_vect) {
	
	/*
	static char tock_message[] = "TOCK";
	s3p_transmit(tock_message, sizeof(tock_message));
	*/
}

int main(void) 
{
	Analog::selectChannel(0);
	
	s3p_init();
	s3p_setbuffers(&Data_in, sizeof(data_in), &Data_out, sizeof(data_out));
	
	TCCR0A = _BV(WGM01); // CTC mode
	TCCR0B =  PRESCALER_1024;
	OCR0A =	249;	
	//TIMSK0 = _BV(OCIE0A); // enable timer interrupt
	
	TCCR1A = 0;
	TCCR1B = _BV(WGM12) | PRESCALER_1024; // CTC mode
	OCR1AH = 0x20;
	OCR1AL = 0x00;
	TIMSK1 = _BV(OCIE1A); // enable timer interrupt
	
	Data_in.r = 255;
	Data_in.g = 160;
	Data_in.b = 32;
	Data_out.r = Data_in.r;
	
	sei(); // set interrupts
	
	int i = 0;
	
    while(1)	
	{
		Analog::startConversion();
		_delay_ms(100);
		Data_out.test = Analog::getValue();
		
		Data_out.r = Data_in.r;
		Data_out.g = Data_in.g;
		Data_out.b = Data_in.b;
	}
}