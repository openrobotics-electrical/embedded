/*
 *  io_utils.h
 *  February 2015
 *  Author: Max Prokopenko
 *  max@theprogrammingclub.com
 */ 

#ifndef IO_UTILS_H
#define IO_UTILS_H

using namespace std;

//------------------VARIABLES

volatile uint8_t analogLow, analogHigh;

//------------------INTERRUPT SERVICE ROUTINES

ISR(ADC_vect) {
	
	analogLow = ADCL;
	analogHigh = ADCH;
}

//------------------FUNCTIONS

void setBitTo(uint8_t bit, uint8_t value, volatile uint8_t *reg) {
	*reg = (value << bit) | (*reg & (0xff - (1 << bit)));
}

bool isBitSet(uint8_t bit, volatile uint8_t *reg) {
	return ((1 << bit) &  *reg) > 0? true : false;
}

//------------------CLASSES

class Bit {
	
	public:

//------CONSTRUCTORS

		Bit() {};
			
		Bit(uint8_t bit, volatile uint8_t *port, volatile uint8_t *dir) {
		
			this->bit = bit;
			this->port = port;
			this->dir = dir; 
		}

//------PUBLIC FUNCTIONS
	
		void set()			{ setBitTo(this->bit, 1, this->port); }		
		void clear()		{ setBitTo(this->bit, 0, this->port); }	
						
		bool isHigh()		{ return *(this->port) & (1 << this->bit); }	
		bool isLow()		{ return !isHigh(); }	
					
		void setAsOutput()	{ setBitTo(bit, 1, dir); }		
		void setAsInput()	{ setBitTo(bit, 0, dir); }
	
	private:

//------VARIABLES

		uint8_t bit;
		volatile uint8_t *dir, *port;
};

class Bus {
	
	public:
		
		Bit *bits;
		int width;
		
		Bus() { this-> width = 0; };
			
		Bus add(Bit b) { 
			
			bits = (Bit *) realloc (++width);
			bits[width - 1] = b;
			return *this;
		}	
};

class Analog {
	
	public:
	
//------FUNCTIONS
	
		static void selectChannel (uint8_t n) {
		
			ADMUX  = 0b01100000 | n; // port n ADC selected
			ADCSRA = 0b10001011; // on, 2x clock
			ADCSRB = 0b00000000; // free running
		}
	
		static void startConversion () { setBitTo(ADSC, 1, &ADCSRA); }
		
		static bool conversionComplete () { return isBitSet(ADSC, &ADCSRA)? false : true; }
};


#endif