/*
 * modular8.cpp
 *
 * Created: 1/16/2016 6:42:19 PM
 *  Author: Maxim
 */ 

void modular8_set_digital_bus(uint8_t out)
{
	PORTD = out & DMASK;
	PORTB = out & BMASK;
}

uint8_t modular8_get_digital_bus()
{
	return (PIND & DMASK) | (PINB & BMASK);
}

void modular8_set_digital_bus_direction(uint8_t dir)
{
	DDRD = dir & DMASK;
	DDRB = dir & BMASK;
}