#ifndef ANALOG_H
#define ANALOG_H

#include <avr/io.h>
#include <avr/interrupt.h>
// #include <stdlib.h>

class Analog {
	
	public:
	
	volatile static uint8_t analogHigh, analogLow;
	
	static void selectChannel (uint8_t n) {
		
		DDRC = DDRC & ~_BV(n);
		ADMUX  =  n; // uses AREF voltage, channel n selected
		ADCSRA = _BV(ADEN) | _BV(ADIE) | 0b111; // on, interrupt enabled, 1/128 clock
		ADCSRB = 0; // free running
	}
	
	static void startConversion () { ADCSRA = _BV(ADSC) | ADCSRA; }
	
	static bool conversionComplete () { return ADCSRA & _BV(ADSC); }
	
	static uint16_t getValue () { return ((Analog::analogHigh << 8)) | (Analog::analogLow); }
};

volatile uint8_t Analog::analogHigh = 0;
volatile uint8_t Analog::analogLow = 0;

ISR(ADC_vect) {
	
	Analog::analogLow = ADCL;
	Analog::analogHigh = ADCH;
}

#endif

/*
	EXAMPLE:
	
	while(1) 
	{	
		Analog::selectChannel(0);
		sei(); // enables interrupts
		uint16_t analog_value = Analog::getValue();
		Analog::startConversion();
	
		// do something with analog_value
		// value will not be valid the first time or two through while loop
	}
*/