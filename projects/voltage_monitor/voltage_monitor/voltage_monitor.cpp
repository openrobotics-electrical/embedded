/*
 * voltage_monitor.cpp
 *
 * Created: 8/8/2015 7:38:58 PM
 *  Author: Maxim
 */ 

#define F_CPU 8000000
#define RLED 0b00010000
#define RLEDMASK 0b11101111
#define GLED 0b00100000
#define GLEDMASK 0b11011111
#define BLED 0b00001000
#define BLEDMASK 0b11110111
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

ISR(ADC_vect) {
	
	analogLow = ADCL;
	analogHigh = ADCH;
}

void setBitTo(uint8_t bit, uint8_t value, volatile uint8_t *reg) {
	*reg = (value << bit) | (*reg & (0xff - (1 << bit)));
}

bool isBitSet(uint8_t bit, volatile uint8_t *reg) {
	return ((1 << bit) &  *reg) > 0? true : false;
}

class Analog {
	
private:
	
	volatile uint8_t analogHigh, analogLow;
	
public:
	
	static void selectChannel (uint8_t n) {
		
		DDRC = _BV(n) & ~DDRC;
		ADMUX  = 0b01100000 | n; // port n ADC selected
		ADCSRA = 0b10001011; // on, 2x clock
		ADCSRB = 0b00000000; // free running
	}
	
	static void startConversion () { setBitTo(ADSC, 1, &ADCSRA); }
	
	static bool conversionComplete () { return isBitSet(ADSC, &ADCSRA)? false : true; }
	
	static uint16_t getValue () { return ((analogHigh << 2) | (analogLow >> 6)); }
};

int main(void)
{
	DDRD = 0xff;
	DDRB = 0xff; 
	DDRC = 0x3f;
	
	int16_t dutyX, dutyY, dutyZ;
	uint8_t out;
	
	while(1) {
		
		PORTB =~ PINB;
		_delay_ms(100);
	}
	
	sei();
	
    while(1)
    {
		dutyX = Analog::getValue() - 512;
		dutyX = dutyX < 0? -1 * dutyX : dutyX;
		Analog::selectChannel(7);
		Analog::startConversion();
		
		dutyZ = 4 * ((dutyX + dutyY) ^ 2);
		
		out = 0;
		
		for(int i = 0; i < 1023; i++) {
			out = i < dutyZ? out | RLED : out & RLEDMASK;  
			out = i > dutyZ? out | GLED : out & GLEDMASK;  
			// out = i < dutyZ? out | BLED : out & BLEDMASK;  
			PORTC = out;
			_delay_us(20);
		}
		
		dutyY = Analog::getValue() - 512;
		dutyY = dutyY < 0? -1 * dutyY : dutyY;
		Analog::selectChannel(6);
		Analog::startConversion();
		
		for(int i = 0; i < 1023; i++) {
			out = i < dutyZ? out | RLED : out & RLEDMASK;
			out = i > dutyZ? out | GLED : out & GLEDMASK;
			// out = i < dutyZ? out | BLED : out & BLEDMASK;  
			PORTC = out;
			_delay_us(20);
		}
    }
}