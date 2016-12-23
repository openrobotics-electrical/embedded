#define F_CPU 8000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "timers.h"
#include "io_manip.h"
//#include <serial.h>
//#include <string.h>

using namespace timers;
using namespace io_manip;

Output servo_en	(&PORTA, PORTA0);
Output tx		(&PORTA, PORTA1);
Input  rx		(&PORTA, PORTA2);
Output txden	(&PORTA, PORTA3);
Output sck		(&PORTA, PORTA4);
Output pwm1		(&PORTA, PORTA5);
Output pwm2		(&PORTA, PORTA6);
Output trig1	(&PORTA, PORTA7);
Input  echo1	(&PORTB, PORTB0);
Output trig2	(&PORTB, PORTB1);
Output echo2	(&PORTB, PORTB2);
// Reset pin on PORTB3

Timer0 timer0(timers::PRESCALER_256);
Timer1 timer1(timers::PRESCALER_1);
Timer2 timer2(timers::PRESCALER_1);

volatile uint16_t timer0_overflows(0);
volatile uint16_t timer0_overflow_limit(0);

ISR(TIMER0_OVF_vect) {
	cli();
	trig2.set();
	timer0_overflows++;
	if(timer0_overflows > timer0_overflow_limit) {
		timer0_overflows = 0;
		TIFR0 |= 1<<OCF0B;
		TIMSK0 = 1<<OCIE0B;
	}
	trig2.clear();
	sei();
}

ISR(TIMER0_COMPA_vect) {}
	
ISR(TIMER0_COMPB_vect) {
	cli();
	trig1.set();
	if (timer0_overflow_limit > 0) {
		TIMSK0 = 1<<TOIE0;
	}
	TCNT0 = 0;
	TIFR0 |= 1<<TOV0;
	echo2.toggle();
	trig1.clear();
	sei();
}
	
volatile uint16_t conversion, period;

void set_frequency_Hz(uint32_t f) {
	cli();
	period = uint32_t(F_CPU) / (f*512);
	/*
	if (period != 15) {
		_delay_ms(1);
		for (uint8_t i=0; i < period; ++i) {
			trig1.toggle();
			trig1.toggle();
		}
		_delay_ms(1);
	}
	*/
	timer0_overflow_limit = period / 256;
	if(timer0_overflow_limit == 0)
		TIMSK0 |= (1<<OCIE0B);
	else {
		TIMSK0 |= 1<<TOIE0;
		TIFR0 |= 1<<TOV0;
	}
	timer0.set_output_compare_B(period % 256);
	sei();
}

void frequency_sweep(uint16_t start, uint16_t stop, uint16_t duration=0) {
	timer0.start();
	uint16_t current_frequency(start);
	while (current_frequency < stop) {
		set_frequency_Hz(current_frequency++);
		_delay_ms(1000);		
	}
	timer0.stop();
}

int main() {
	TOCPMSA0 = 0; // TOCC3,TOCC2,TOCC1,TOCC0 output pins use timer0
	TOCPMSA1 = 0; // TOCC7,TOCC6,TOCC5,TOCC4 output pins use timer0
	TOCPMCOE = 0; //1<<TOCC7OE;
	// timer0.set_output_compare_A(254U);
	// timer0.set_output_compare_B(50U);
	TIMSK0 |= 1<<TOIE0;
	sei();
	timer0.start();
	uint16_t pattern[] = {25,50,100,200};
	uint8_t inc(0);
	while (1) {
		inc %= 4;
		inc++;
		TCNT0 = 0;
		timer0.start();
		set_frequency_Hz(pattern[inc]);
		_delay_ms(200);
		timer0.stop();
		_delay_ms(200);
		sck.toggle();
	}
}
