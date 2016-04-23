#include <analog.h>

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

volatile bool Analog::newValue = false;
volatile uint8_t Analog::analogHigh = 0;
volatile uint8_t Analog::analogLow = 0;
volatile uint8_t Analog::lastChannel = 0; // Channels 0-7 for ATmega328P

ISR(ADC_vect) 
{	
	Analog::analogLow = ADCL;
	Analog::analogHigh = ADCH;
	Analog::newValue = true;
}

void Analog::selectChannel (uint8_t n) 
{		
	DDRC = DDRC & ~_BV(n);
	ADMUX  = _BV(REFS0) | n; // AREF = AVCC
	//ADMUX  = n; // uses AREF voltage, channel n selected
	ADCSRA = /*_BV(ADATE) |*/ _BV(ADEN) | _BV(ADIE) | 0b111; // on, interrupt enabled, 1/128 clock
	ADCSRB = 0; // free running
}

void Analog::autoTriggerEnable(bool enabled) 
{
	ADCSRA = enabled? ADCSRA | _BV(ADATE) : ADCSRA & ~_BV(ADATE);
}
	
void Analog::startConversion() 
{ 
	newValue = false;
	ADCSRA |= _BV(ADSC); 
}
	
void Analog::stopConversion() { ADCSRA &= ~_BV(ADSC); }
	
bool Analog::conversionComplete() { return ADCSRA & _BV(ADSC); }
	
bool Analog::newValueAvailable() { return newValue; }
	
uint16_t Analog::getValue() 
{
	return ((Analog::analogHigh << 8)) | (Analog::analogLow); 
}
