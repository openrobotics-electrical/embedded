/*
 * Blunderbot_rev1.cpp
 *
 * Created: 4/19/2016 8:35:22 PM
 * Author : Maxim
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint16_t echo1;

#define PRESCALER_1 1
#define PRESCALER_8 2
#define PRESCALER_64 3
#define PRESCALER_256 4
#define PRESCALER_1024 5

ISR(PCINT1_vect) 
{
	cli();
	if(PINB & (1<<PINB0)) {
		PORTA |= (1<<PORTA4);
		TCNT2 = 0;
		TCCR2B = PRESCALER_8;
	} else { 
		PORTA &= ~(1<<PORTA4);
		TCCR2B = 0;
		echo1 = TCNT2;
	}
	sei();
}

int main(void)
{
	OSCCAL0 = 0x47;
	
    DDRA = 0xff;
	DDRB = 0x02;
	
	TCCR1A = (1<<COM1B1) | (1<<COM1A1) | (1<<WGM11);
	TCCR1B = (1<<WGM13) | (1<<WGM12) | PRESCALER_8;
	TOCPMSA1 = (1<<TOCC5S0) | (1<<TOCC4S0);
	TOCPMCOE = (1<<TOCC5OE) | (1<<TOCC4OE);
	OCR1B = 1500; // PWM1 us
	OCR1A = 1500; // PWM2 us 
	ICR1 = 20000; // Period us
	
	TCCR2A = 0;
	TCCR2B = 0;
	
	PCMSK1 = (1<<PCINT8);
	GIMSK = (1<<PCIE1);
	sei();
	
	uint16_t servo_array[] = {1400, 1500, 1600};
	
    while (1) 
    {
		if(!(PINB & 1))
			PORTA |= (1<<PORTA7);
		_delay_us(20);
		PORTA &= ~(1<<PORTA7);;
		_delay_ms(20);
		uint16_t pulse = echo1 / 3 + 800;
		pulse = pulse > 2200? 2200 : pulse;
		pulse = pulse < 800? 800 : pulse;
		/*
		//OCR1B = OCR1A = pulse;
		PORTA |= (1<<PORTA0);
		_delay_ms(10);
		TCCR1B |= PRESCALER_8;
		_delay_ms(80);
		TCCR1B &= !PRESCALER_8;
		_delay_ms(10);
		//PORTA &= ~(1<<PORTA0);
		*/
    }
}

