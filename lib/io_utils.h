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

class Analog {
	
	/*	class last edited April 29th, 2015	*/
	
	public:
	
//------STATIC FUNCTIONS
	
		static void selectChannel (uint8_t n) {
		
			setBitTo(n, 0, &PORTC);
			ADMUX  = 0b01100000 | n; // port n ADC selected
			ADCSRA = 0b10001011; // on, 2x clock
			ADCSRB = 0b00000000; // free running
		}
	
		static void startConversion () { setBitTo(ADSC, 1, &ADCSRA); }
	
		static bool conversionComplete () { return isBitSet(ADSC, &ADCSRA)? false : true; }
		
		static uint16_t getValue () { return ((analogHigh << 2) | (analogLow >> 6)); }
		
		static void test () {
			
			// selects PORTC0 (Nano: A0) and outputs ADC value
		
			sei(); // enable interrupts
			char str[20];
		
			while(1) {
			
				Analog::selectChannel(0);
				Analog::startConversion();
							
				// may need some setup time if only calling once
			
				sprintf(str, "ADC value: %u\n", Analog::getValue());
				USART_Send_string(str);
			}
		}
};


class Bit {
	
	/*	class last edited April 29th, 2015	*/
	
	public:

//------CONSTRUCTORS

		Bit() {};
			
		Bit(uint8_t bit, uint8_t port) {
		
			this->bit = bit;
			
			if(port == PORTB) {
				
				this->port = &PORTB;
				this->direction = &DDRB;
				this->input = &PINB;
				
			} else if(port == PORTC) {
				
				this->port = &PORTC;
				this->direction = &DDRC;
				this->input = &PINC;
				
			} else {
				
				this->port = &PORTD;
				this->direction = &DDRD;
				this->input = &PIND;
			}
		}

//------PUBLIC FUNCTIONS
	
		void set()			{ setBitTo(this->bit, 1, this->port); }		
		void clear()		{ setBitTo(this->bit, 0, this->port); }	
						
		bool isHigh()		{ return *(this->input) & (1 << this->bit); }	
		bool isLow()		{ return !isHigh(); }
					
		void setAsOutput()	{ setBitTo(bit, 1, direction); }		
		
		void setAsPullUpInput()	{ 
			
			setBitTo(bit, 0, direction); 
			set();
		}
		
		void setAsHighZInput() { 
		
			setBitTo(bit, 0, direction);
			clear();
		}
		
		static void test() {
			
			// blinks PORTB5 (Nano light) with rate controlled by PORTD2
			
			Bit blinky(5, PORTB);
			Bit rateSelect(2, PORTD);
			
			blinky.setAsOutput();
			rateSelect.setAsPullUpInput();
			
			while(1) {
				
				blinky.set();
				_delay_ms(200);
				blinky.clear();
				if(rateSelect.isHigh())
					_delay_ms(200);
				else
					_delay_ms(500);
			}
		}
	
	private:

//------VARIABLES

		uint8_t bit;
		volatile uint8_t *direction, *port, *input;
};

class Bus {
	
	//___ IN DEVELOPMENT ___
	
	public:
		
		Bit *bits;
		int width;
		
		Bus() { this-> width = 0; };
			
		Bus add(Bit b) { 
			
			bits = (Bit *) realloc (bits, ++width * sizeof(Bit));
			bits[width - 1] = b;
			return *this;
		}	
};

#endif