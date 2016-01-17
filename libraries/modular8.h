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

#define BMASK			0b00000011
#define DMASK			0b11111100

#define PRESCALER_1		0b001
#define PRESCALER_8		0b010
#define PRESCALER_64	0b011
#define PRESCALER_256	0b100
#define PRESCALER_1024	0b101

#ifndef _BV
#define _BV(x) (1 << (x))
#endif

void modular8_set_digital_bus(uint8_t out);

uint8_t modular8_get_digital_bus();

void modular8_set_digital_bus_direction(uint8_t dir);

#endif
