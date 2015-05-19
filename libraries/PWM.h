#ifndef PWM_H
#define PWM_H

/*
 * PWM.h
 *
 * February 2015
 *  Author: Max Prokopenko
 *  max@theprogrammingclub.com
 */

using namespace std;

class PWM {
	private:
		unsigned short PWM_number;
		unsigned short savedState;
	public:
		PWM(unsigned short i);
		void init();
		void setDuty(float f);
		void setSpeed(float f);
		unsigned int read();
		void start();
		void stop();
};

#include "F_CPU.h"
#include <avr/io.h>

#define TIMER_1_FREQUENCY 250 // Hz
#define TIMER_2_FREQUENCY 5000 // Hz

PWM::PWM (unsigned short u) {
	PWM_number = u;
	init();
}

void PWM::init() {
	PRR = 0; // disable all power reduction
	
	if(this->PWM_number == 0) { // init counter0 on pin D6
		OCR0A = 0x01;
		OCR0B = 0x03;
		TCCR0A = (0 << COM0A1) | (1 << COM0A0) | (1 << WGM01) | (1 << WGM00);
		// TIMSK0 = (1 << OCIE1A) | (1 << OCIE1B);
		// TIMSK0 |= (1 << TOIE0);
	} else if (this->PWM_number == 1) {
		OCR1A = F_CPU / TIMER_1_FREQUENCY;
		OCR1B = 0;
		TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << WGM11) | (0 << WGM10);
		TIMSK1 |= (1 << OCIE1A) | (1 << TOIE1); // timer1 compare, overflow 
	} else if (this->PWM_number == 2) {
		OCR2A = uint8_t(F_CPU / 32 / TIMER_2_FREQUENCY);
		// OCR2B =  83; // (uint8_t)F_CPU / (TIMER_2_FREQUENCY * 64);
		TCCR2A = (1 << COM2B1) | (0 << COM2B0) | (1 << WGM21) | (1 << WGM20); // fast PWM
		TIMSK2 = 0;
		// TIMSK2 |= (1 << OCIE2B) | (1 << OCIE2A) | (1 << TOIE2); // timer2 compare, overflow		
	}
}

unsigned int PWM::read() {return 0;}

void PWM::start() {
	if(this->PWM_number == 0) {
		TCCR0B = (0 << COM0B1) | (0 << COM0B0) | (1 << WGM02) | (1 << CS00);
	} else if (this->PWM_number == 1) {
		TCCR1B = (0 << WGM13) | (0 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);
	} else if (this->PWM_number == 2) {
		TCCR2B = (1 << WGM22) | (0 << CS22) | (1 << CS21) | (1 << CS20);
	}
}

void PWM::stop() {
	if(this->PWM_number == 0) {
		TCCR0B = 0;
	} else if(this->PWM_number == 1) {
		TCCR1B = 0;
	} else if(this->PWM_number == 2) {
		TCCR2B = 0;
	}
}

void PWM::setDuty(float f) {
	if(this->PWM_number == 2) {
		OCR2B = (uint8_t)(OCR2A * (1 - f));
	}
}

#define MAX_SPEED 0.5f

void PWM::setSpeed(float f) {
	if(this->PWM_number == 2) {
		if(f > MAX_SPEED)
			f = MAX_SPEED;
		else if(f < -MAX_SPEED)
			f = -MAX_SPEED;
		OCR2B = (uint8_t)(OCR2A * (0.5 - f / 2));
	}
}

#endif