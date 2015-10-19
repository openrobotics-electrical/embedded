#ifndef MODULAR8_H
#define MODULAR8_H
/*
 * modular8.h
 *
 * Created: 10/18/2015 7:19:53 PM
 *  Author: Maxim
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define BMASK 0b00000011
#define DMASK 0b11111100

void set_digital_bus(uint8_t out) {
	
	PORTD = out & DMASK;
	PORTB = out & BMASK;
}

uint8_t get_digital_bus() {
	
	return (PIND & DMASK) | (PINB & BMASK);
}

void set_digital_bus_direction(uint8_t dir) {
	
	DDRD = dir & DMASK;
	DDRB = dir & BMASK;
}

#endif
