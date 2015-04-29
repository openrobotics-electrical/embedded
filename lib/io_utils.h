/*
 *  io_utils.h
 *  February 2015
 *  Author: Max Prokopenko
 *  max@theprogrammingclub.com
 */ 

#ifndef IO_UTILS_H
#define IO_UTILS_H

using namespace std;

void setBitTo(uint8_t bit, uint8_t value, volatile uint8_t *reg) {
	*reg = (value << bit) | (*reg & (0xff - (1 << bit)));
}

class Bit {
	public:
	
		Bit(uint8_t bit, volatile uint8_t *port, volatile uint8_t *dir) {
			this->bit = bit;
			this->port = port;
		}
	
		uint8_t bit;
		volatile uint8_t *port;
};

class Output : Bit {
	public:
	
		Output(uint8_t bit, volatile uint8_t *port, volatile uint8_t *dir) 
		: Bit (bit, port, dir) {
			setBitTo(bit, 1, dir);
		}
	
		void set(void) {
			setBitTo(this->bit, 1, this->port);
		}
		
		void clear(void) {
			setBitTo(this->bit, 0, this->port);
		}		
};

class Input : Bit {	
	public:
	
		Input(uint8_t bit, volatile uint8_t *port, volatile uint8_t *dir)
		: Bit (bit, port, dir) {
			setBitTo(bit, 0, dir);
		}
				
		bool isHigh(void) {
			return *(this->port) & (1 << this->bit);
		}
		
		bool isLow(void) {
			return !isHigh();
		}
};

#endif