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
	// automatically received data
} Data_in;

volatile struct data_out
{
	// automatically transmitted data
} Data_out;

ISR(TIMER0_COMPA_vect) {}

ISR(TIMER1_COMPA_vect) {}

volatile uint16_t i = 0, j = 0;
volatile uint16_t range[3];
volatile uint8_t left = 0, right = 0;

void check_range(volatile uint16_t *range) {
	
	PORTC = 0;
	_delay_ms(100);
	PORTC = _BV(PORTC4);
	_delay_us(20);
	PORTC = 0;
	
	*range = 0;
	volatile uint16_t timeout = 0;
	#define TIMEOUT_MICRO 10000
	
	timeout = 0;
	
	while(!(PINC & _BV(PORTC5)) && (timeout < TIMEOUT_MICRO))
	{
		_delay_us(1);
		timeout++;
	}
	
	timeout = 0;
	
	while((PINC & _BV(PORTC5)) && (timeout < TIMEOUT_MICRO))
	{
		*range = *range + 1;
		_delay_us(1);
		timeout++;
	}
}

void move() {
	
	_delay_ms(200);
	
	for(j = 0; j < 10; j++)
	{
		PORTB = 0x3;
		
		_delay_us(1000);
		
		for(i = 0; i != 255; i++)
		{
			if(i > right)
			PORTB = PORTB & ~_BV(0);
			if(i > left)
			PORTB = PORTB & ~_BV(1);
			_delay_us(1);
		}
		
		PORTB = 0;
		_delay_ms(10);
	}
	
	_delay_ms(200);
}

void back() {
	
	left = 117;
	right = 145;
	
	move();
}

void forward() {
	
	left = 151;
	right = 107;
	
	move();
}

void turn_right() {
	
	left = 146;
	right = 143;
	
	move();
}

void turn_left() {
	
	left = 115;
	right = 105;
	
	move();
}

int main(void) 
{
	Analog::selectChannel(0);
	
	s3p_init();
	s3p_setbuffers(&Data_in, sizeof(Data_in), &Data_out, sizeof(Data_out));
	
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
	
	sei(); // set interrupts
	
	DDRB = 0xff;
	DDRD = 0xff;
	DDRC = _BV(PORTC4);

    while(1)	
	{	
		
		check_range(&(range[0]));
		turn_right();
		check_range(&(range[1]));
		
		if(range[1] > range[0])
		{
			if(range[1] > 300)
				forward();
		}
		else 
		{
			turn_left();
			
			if(range[0] > 300)
				forward();
			else
			{
				turn_left();
				turn_left();
			}
		}
	}
}