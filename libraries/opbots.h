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
	OUT_OF_MEMORY=2,
	BUFFER_OVERFLOW=3,
	OVER_VOLTAGE=4,
	OVER_CURRENT=5
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
	volatile static bool new_value;
	volatile static uint8_t analogHigh;
	volatile static uint8_t analog_low;
	volatile static uint8_t last_channel;
	
	static void select_channel(uint8_t n) {
#ifdef _AVR_ATTINY841_H_INCLUDED
		//! TODO		
#endif
#ifndef _AVR_ATTINY841_H_INCLUDED
		DDRC = DDRC & ~(1<<n);
		ADMUX  = (1<<REFS0) | n; // AREF = AVCC
#endif
		ADCSRA = 1<<(ADEN) | 1<<(ADIE) | 0b111; // on, interrupt enabled, 1/128 clock
		ADCSRB = 0; // free running
	}
	static void auto_trigger_enable(bool enabled) {
		ADCSRA = enabled? ADCSRA | 1<<(ADATE) : ADCSRA & ~1<<(ADATE);
	}
	static void start_conversion() {
		new_value = false;
		ADCSRA |= 1<<(ADSC); 
	}
	static void stop_conversion() { ADCSRA &= ~1<<(ADSC); }
	static bool inline conversion_complete() { return new_value; }
	static uint16_t inline get_value () { 
		return ((Analog::analogHigh << 8)) | (Analog::analog_low); 
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

volatile bool Analog::new_value = false;
volatile uint8_t Analog::analogHigh = 0;
volatile uint8_t Analog::analog_low = 0;
volatile uint8_t Analog::last_channel = 0; // Channels 0-7 for ATmega328P

template<typename T>
class SimpleBuffer {
protected:
	T* buffer;
	const uint8_t size;
	uint8_t head;

public:
	SimpleBuffer(const uint8_t buffer_size) : size(buffer_size), head(0) {
		buffer = (T*)malloc(size);
		if (buffer == nullptr) {
			//TODO _raise_error(ErrorType::OUT_OF_MEMORY);
		}
	}
	inline void push(T element_copy) {
		if (head < size) {
			*(buffer+head) = element_copy;
		} else {
			// TODO _raise_error(ErrorType::BUFFER_OVERFLOW);
		}
	};
	inline void set(T* element_array,uint8_t n) {
		if (n <= size) {
			memcpy(buffer+head,element_array,n);
			head = n;
		} else {
			// TODO _raise_error(ErrorType::BUFFER_OVERFLOW);
		}
	};
	inline uint8_t count() { return head; }
	inline T pop() { return buffer[head]; }
};

const uint8_t TX_BUFFER_SIZE(32);
const uint8_t RX_BUFFER_SIZE(32);

class Serial {
public:
	volatile static char transmitting[];
	volatile static uint8_t tx_index, tx_head, chars_left;
#ifdef _AVR_ATTINY841_H_INCLUDED
	static Output* txden[2];
#endif
#ifndef _AVR_ATTINY841_H_INCLUDED
	static Output* txden[1];
#endif
	
	static void init(const uint32_t baud_rate, const uint8_t serial_port=0) {
		uint8_t offset = 0;
		const uint16_t baud = (F_CPU / 8 / baud_rate) - 1;
#ifdef _AVR_ATTINY841_H_INCLUDED 
		if (serial_port == 1) {
			offset = 0x10;
		} 
#endif
		*(&UBRR0H+offset) = baud / 0xFF;
		*(&UBRR0L+offset) = baud % 0xFF;
		*(&UCSR0A+offset) = 1<<(U2X0);
		*(&UCSR0B+offset) = 1<<(TXCIE0) | 1<<(RXCIE0) | 1<<(RXEN0) | 1<<(TXEN0);
		*(&UCSR0C+offset) = 1<<(UCSZ01) | 1<<(UCSZ00);
	}
	template<typename T>
	static void transmit(T* buffer, uint8_t char_count, const uint8_t serial_port=0) {
		// sends between 1 and 255 chars if large enough buffer allocated
		// does not prevent against buffer overwrites, increase
		// buffer size if needed
		cli();
		// TODO optimize all this shit
		for (uint8_t chars_added(0); chars_added < char_count; chars_added++) {
			transmitting[tx_index] = buffer[chars_added];
			tx_index = (tx_index + 1) % TX_BUFFER_SIZE;
		}
		if (serial_port == 0) {
			UDR0 = transmitting[tx_head];
		}
#ifdef _AVR_ATTINY841_H_INCLUDED 
		else if (serial_port == 1) {
			UDR1 = transmitting[tx_head];
		}
#endif
		tx_head = (tx_head + 1) % TX_BUFFER_SIZE;
		chars_left += (char_count - 1);
		if (Serial::txden[serial_port] != nullptr) {
			Serial::txden[serial_port]->set();
		}
		sei();
	}
	static void set_txden_pin(Output& txden_pin, const uint8_t serial_port=0) {
		Serial::txden[serial_port] = &txden_pin;
	}
	static inline void service_tx_interrupt(const uint8_t serial_port=0) {
		cli();
		if (Serial::chars_left > 0) {
			if (serial_port == 0) {
				UDR0 = Serial::transmitting[Serial::tx_head];
			}
#ifdef _AVR_ATTINY841_H_INCLUDED 
			else if (serial_port == 1) {
				UDR1 = Serial::transmitting[Serial::tx_head];
			}
#endif
			Serial::tx_head = (Serial::tx_head + 1) % TX_BUFFER_SIZE;
			Serial::chars_left--;
		} else {
			if (Serial::txden[serial_port] != nullptr) {
				Serial::txden[serial_port]->clear();
			}
		}
		sei();
	}
	static inline void service_rx_interrupt(const uint8_t serial_port=0) {
		cli();
		if (serial_port == 0) {
			UCSR0A &= ~(1<<RXC0); // clear flag -- do I need this?
		} 
#ifdef _AVR_ATTINY841_H_INCLUDED 
		else if (serial_port == 1) {
			UCSR1A &= ~(1<<RXC1); // clear flag -- do I need this?
		}
#endif
		sei();
	}
};

volatile char Serial::transmitting[TX_BUFFER_SIZE];
volatile uint8_t Serial::tx_index(0), Serial::tx_head(0), Serial::chars_left(0);
Output* Serial::txden[];

} /* end of namespace opbots */

using namespace opbots;

ISR(ADC_vect) {
	cli();
	Analog::analog_low = ADCL;
	Analog::analogHigh = ADCH;
	Analog::new_value = true;
	sei();
};

#ifdef _AVR_ATTINY841_H_INCLUDED
ISR(USART0_TX_vect) { Serial::service_tx_interrupt(0); }
ISR(USART1_TX_vect) { Serial::service_tx_interrupt(1); }
#endif
#ifndef _AVR_ATTINY841_H_INCLUDED
ISR(USART_TX_vect) { Serial::service_tx_interrupt(0); }
#endif

#ifdef _AVR_ATTINY841_H_INCLUDED
ISR(USART0_RX_vect) { Serial::service_rx_interrupt(0); }
ISR(USART1_RX_vect) { Serial::service_rx_interrupt(1); }
#endif
#ifndef _AVR_ATTINY841_H_INCLUDED
ISR(USART_RX_vect) { Serial::service_rx_interrupt(0); }
#endif
	
#endif /* OPBOTS_H */