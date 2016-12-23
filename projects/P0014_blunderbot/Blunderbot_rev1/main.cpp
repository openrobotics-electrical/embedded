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
#include <stdio.h>
#include "timers.h"
#include "io_manip.h"
#include <serial.h>
#include <string.h>

volatile uint32_t echo1_time;

using io_manip::Output;
using io_manip::Input;
	
Output servo_en	(&PORTA, PA0);
Output tx		(&PORTA, PA1);
Input  rx		(&PORTA, PA2);
Output txden	(&PORTA, PA3);
Output sck		(&PORTA, PA4);
Output pwm1		(&PORTA, PA5);
Output pwm2		(&PORTA, PA6);
Output trig1	(&PORTA, PA7);
Input  echo1	(&PORTB, PB0);
Output trig2	(&PORTB, PB1);
Input  echo2	(&PORTB, PB2);
// Reset pin on PB3

using timers::Timer0;
using timers::Timer1;
using timers::Timer2;
using timers::Prescaler;

Timer1 timer1(Prescaler::PRESCALER_8);
Timer2 timer2(Prescaler::PRESCALER_8);

ISR(PCINT1_vect) {
	cli();
	if(echo1.is_set()) {
		sck.set();
		timer2.clear();
		timer2.start();
	} else { 
		sck.clear();
		timer2.stop();
		echo1_time = (static_cast<uint32_t>(timer2.count()) * 331) / 2000; // ms
	}
	sei();
}

uint16_t sample_distance() {
	if(!echo1.is_set())
		trig1.set();
	_delay_us(20);
	trig1.clear();
	_delay_ms(100);
	return echo1_time;
}

/*
const uint16_t travel_time(150);
const uint16_t wait_time(150);

void move_right() {
	OCR1A = 2000;
	OCR1B = 2000;
	servo_en.set();
	_delay_ms(wait_time);
	timer1.start();
	_delay_ms(travel_time + 10);
	timer1.stop();
	_delay_ms(wait_time);
	servo_en.clear();
}

void move_left() {
	OCR1A = 1100;
	OCR1B = 1100;
	servo_en.set();
	_delay_ms(wait_time);
	timer1.start();
	_delay_ms(travel_time);
	timer1.stop();
	_delay_ms(wait_time);
	servo_en.clear();
}

void move_forward() {
	OCR1A = 1800;
	OCR1B = 1100;
	servo_en.set();
	_delay_ms(wait_time);
	timer1.start();
	_delay_ms(2 * travel_time);
	timer1.stop();
	_delay_ms(wait_time);
	servo_en.clear();
}
*/
char message[16];

int main(void) {
	OSCCAL0 = 0x47; // Calibrate 8 MHz on-chip oscillator

	// Route timer1 to PA5 and PA4
	TOCPMSA1 = (1<<TOCC5S0) | (1<<TOCC4S0);
	// Enable output compare on PA5 and PA4
	TOCPMCOE = (1<<TOCC5OE) | (1<<TOCC4OE);
	
	ICR1 = 20000; // Period us
	
	TCCR2A = 0;
	TCCR2B = 0;
	
	PCMSK1 = (1<<PCINT8);
	GIMSK = (1<<PCIE1);
	sei();
	
	uint16_t last_distance = 0;
	
	serial_init();
    while (1) 
    {
		txden.set();		
		last_distance = sample_distance();
		//for (auto &c : message)
		//	c = 0;
		cli();
		message[0] = (char)((last_distance / 10000) % 10) + '0';
		message[1] = (char)((last_distance / 1000) % 10) + '0';
		message[2] = (char)((last_distance / 100) % 10) + '0';
		message[3] = (char)((last_distance / 10) % 10) + '0';
		message[4] = (char)(last_distance % 10) + '0';
		message[5] = '\n';
		sei();
		//sprintf(message, "[%u]\n", last_distance);
		//sprintf(message, "DOPE! yeahh\n");
		
		serial_transmit(message, 6);
		sck.toggle();
		//_delay_ms(500);
		/*
		uint16_t distance1, distance2, distance3;
		bool choice1, choice2, choice3;
		
		distance1 = multi_sample();
		move_right();
		distance2 = multi_sample();
		move_right();
		distance3 = multi_sample();
		
		choice1 = (distance1 > distance2 && distance1 > distance3);
		choice2 = (distance2 > distance1 && distance2 > distance3);
		choice3 = (!choice1 && !choice2);
		
		if (choice1) {
			move_left();
			move_left();
			if (distance1 > last_distance)
				move_forward();
		}
		if (choice2) {
			move_left();
			if (distance2 > last_distance)
			move_forward();
		}
		if (choice3) {
			if (distance3 > last_distance)
			move_forward();
		} 
		*/
    }
}

