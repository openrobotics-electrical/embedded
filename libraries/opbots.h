#ifndef OPBOTS_H
#define OPBOTS_H

#define F_CPU 16000000
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* Basic allocators for classes */
void * operator new(size_t n) {
	void * const p = malloc(n);
	// TODO: handle p == 0
	return p;
}

void operator delete(void * p) {
	free(p);
}

namespace opbots {
	
enum ErrorType {
	NULL_POINTER=1,
	OVER_VOLTAGE=2,
	OVER_CURRENT=3
};
	
class GPIO {
protected:
	volatile uint8_t *const port_out, *const port_dir, *const port_in;
	const uint8_t pin;
	const uint8_t pin_mask;
	
	GPIO(volatile uint8_t *const io_port, const uint8_t io_pin) :
		port_out(io_port), port_dir(io_port-1U), port_in(io_port-2U),
		pin(io_pin), pin_mask(1<<io_pin) {}
	virtual ~GPIO() {}
	
public:	
	bool inline is_set() { return *port_in & pin_mask; }
};

class Output : public GPIO {
public:
	Output(volatile uint8_t *const io_port, const uint8_t io_pin) 
			: GPIO(io_port, io_pin) {
		// Set output low
		*port_out &= ~pin_mask;
		// Set pin in port to output
		*port_dir |= pin_mask;
	};
	~Output() {}
		
	void inline toggle() { *port_out ^= pin_mask; }
	void inline set() { *port_out |= pin_mask; }
	void inline clear() { *port_out &= ~pin_mask; }
};

class Input : public GPIO {
public:
	Input(volatile uint8_t *const io_port, const uint8_t io_pin)
			: GPIO(io_port, io_pin) {
		// Set pin in port to input;
		*port_dir &= ~pin_mask;
	};
	~Input() {}
};

class Analog {
public:
	volatile static bool newValue;
	volatile static uint8_t analogHigh;
	volatile static uint8_t analogLow;
	volatile static uint8_t lastChannel;
	
	static void selectChannel(uint8_t n) {
		DDRC = DDRC & ~_BV(n);
		ADMUX  = _BV(REFS0) | n; // AREF = AVCC
		ADCSRA = /*_BV(ADATE) |*/ _BV(ADEN) | _BV(ADIE) | 0b111; // on, interrupt enabled, 1/128 clock
		ADCSRB = 0; // free running
	}
	static void autoTriggerEnable(bool enabled) {
		ADCSRA = enabled? ADCSRA | _BV(ADATE) : ADCSRA & ~_BV(ADATE);
	}
	static void startConversion() {
		newValue = false;
		ADCSRA |= _BV(ADSC); 
	}
	static void stopConversion() { ADCSRA &= ~_BV(ADSC); }
	static bool inline conversionComplete() { return newValue; }
	static uint16_t inline getValue () { 
		return ((Analog::analogHigh << 8)) | (Analog::analogLow); 
	}
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
};

volatile bool Analog::newValue = false;
volatile uint8_t Analog::analogHigh = 0;
volatile uint8_t Analog::analogLow = 0;
volatile uint8_t Analog::lastChannel = 0; // Channels 0-7 for ATmega328P

const uint8_t TX_BUFFER_SIZE(16);
const uint8_t RX_BUFFER_SIZE(16);

class Serial {
public:
	volatile static char transmitting[];
	volatile static uint8_t tx_index, tx_head, chars_left;
	volatile static char receiving[];
	volatile static uint8_t rx_index, rx_head, chars_unread;
	
	static void init() {
		UBRR0H = 0;
		UBRR0L = 16; // BAUD 115200
		UCSR0A = _BV(U2X0);
		UCSR0B = _BV(TXCIE0) | _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
		UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	}
	static void transmit(char* s, uint8_t char_count) {
		// sends between 1 and 255 chars if large enough buffer allocated
		// does not prevent against buffer overwrites, increase
		// buffer size if needed
		cli();
		for (uint8_t chars_added(0); chars_added < char_count; chars_added++) {
			transmitting[tx_index] = s[chars_added];
			tx_index = (tx_index + 1) % TX_BUFFER_SIZE;
		} 
		UDR0 = transmitting[tx_head];
		tx_head = (tx_head + 1) % TX_BUFFER_SIZE;
		chars_left += (char_count - 1);
		sei();
	}
	static char get_char(bool* error_flag) {
		if (chars_unread > 0) {	
			*error_flag = false;
			return 	receiving[rx_head];
			rx_head = (rx_head + 1) % RX_BUFFER_SIZE;
			chars_unread--;
		} else {
			*error_flag = true;
			return '\0';
		}
	}
};

volatile char Serial::transmitting[TX_BUFFER_SIZE];
volatile char Serial::receiving[RX_BUFFER_SIZE];
volatile uint8_t Serial::tx_index(0), Serial::tx_head(0), Serial::chars_left(0);
volatile uint8_t Serial::rx_index(0), Serial::rx_head(0), Serial::chars_unread(0);
} /* end of namespace opbots */

using namespace opbots;

ISR(ADC_vect) {
	cli();
	Analog::analogLow = ADCL;
	Analog::analogHigh = ADCH;
	Analog::newValue = true;
	sei();
};

ISR(USART_TX_vect) {
	cli();
	if (Serial::chars_left > 0) {
		UDR0 = Serial::transmitting[Serial::tx_head];
		Serial::tx_head = (Serial::tx_head + 1) % TX_BUFFER_SIZE;
		Serial::chars_left--;
	}
	sei();
};

ISR(USART_RX_vect) {
	cli();
	UCSR0A &= ~_BV(RXC0); // clear flag
	Serial::receiving[Serial::rx_index] = UDR0;
	Serial::rx_index = (Serial::rx_index + 1) % RX_BUFFER_SIZE;
	Serial::chars_unread++;
	sei();
};

#endif /* OPBOTS_H */