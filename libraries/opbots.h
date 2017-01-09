#ifndef OPBOTS_H
#define OPBOTS_H

#define F_CPU 16000000
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

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

void* (*_error_function)(void);
void* (*_error_led_function)(void);

enum ErrorType {
	NULL_POINTER=1,
	OUT_OF_MEMORY=2,
	BUFFER_OVERFLOW=3,
	OVER_VOLTAGE=4,
	OVER_CURRENT=5
};

void _raise_error(ErrorType error) {
	cli(); // Disable all interrupts
	// Disable all power outputs
	/*
	for (int i=1; i<=6; ++i) enable[i].clear();
	// Flash error code on err_led
	while (1) {
		for (int i=0; i < uint8_t(error); ++i) {
			err_led.set();
			_delay_ms(delay_time);
			err_led.clear();
			_delay_ms(5*delay_time);
		}
		_delay_ms(10*delay_time);
	}
	*/
}
	
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
};

volatile bool Analog::new_value = false;
volatile uint8_t Analog::analogHigh = 0;
volatile uint8_t Analog::analog_low = 0;
volatile uint8_t Analog::last_channel = 0; // Channels 0-7 for ATmega328P

template<typename T>
struct SimpleBuffer {
	T* buffer;
	const uint8_t size;
	uint8_t head;
	uint8_t tail;
	
	SimpleBuffer(const uint8_t buffer_size) : size(buffer_size) {
		buffer = (T*)malloc(size);
	}
};

class Serial {
public:
#ifdef _AVR_ATTINY841_H_INCLUDED
	static Output* txden[2];
#endif
#ifndef _AVR_ATTINY841_H_INCLUDED
	static Output* txden[1];
	static SimpleBuffer<char> tx[1];
	static SimpleBuffer<char> rx[1];
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
		*(&UCSR0B+offset) = 1<<(TXCIE0) |/* 1<<(RXCIE0) | 1<<(RXEN0) |*/ 1<<(TXEN0);
		*(&UCSR0C+offset) = 1<<(UCSZ01) | 1<<(UCSZ00);
	}
	template<typename T>
	static void transmit(T* buffer, uint8_t char_count, const uint8_t serial_port=0) {
		// sends between 1 and 255 chars if large enough buffer allocated
		// does not prevent against buffer overwrites, increase
		// buffer size if needed
		cli();
		if (Serial::txden[serial_port] != nullptr) {
			Serial::txden[serial_port]->set();
		}
		if (serial_port == 0) {
			if (char_count > tx[0].size) {
				_raise_error(ErrorType::BUFFER_OVERFLOW);
			}
			memcpy(tx[0].buffer, buffer, char_count);
			tx[0].head = 0;
			tx[0].tail = char_count;
			UDR0 = tx[0].buffer[tx[0].head];
		}
#ifdef _AVR_ATTINY841_H_INCLUDED 
		else if (serial_port == 1) {
			UDR1 = transmitting[tx_head];
		}
#endif
		sei();
	}
	static void set_txden_pin(Output& txden_pin, const uint8_t serial_port=0) {
		Serial::txden[serial_port] = &txden_pin;
	}
	static char empty() {
		return rx[0].head > 0;
	} 
	static char get_char() {
		if (!empty()) {
			rx[0].head--;
			return rx[0].buffer[rx[0].head];
		}
	}
	static inline void service_tx_interrupt(const uint8_t serial_port=0) {
		cli();
		if (tx[serial_port].head < tx[serial_port].tail) {
			if (serial_port == 0) {
				tx[serial_port].head++;
				UDR0 = char(tx[serial_port].buffer[tx[serial_port].head]);
			}
#ifdef _AVR_ATTINY841_H_INCLUDED 
			else if (serial_port == 1) {
				UDR1 = 'x';
			}
#endif
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
			if (rx[serial_port].head < rx[serial_port].size) {
				rx[serial_port].buffer[rx[serial_port].head] = UDR0;
				rx[serial_port].head++;
			} else {
				_raise_error(ErrorType::BUFFER_OVERFLOW);
			}
		} 
#ifdef _AVR_ATTINY841_H_INCLUDED 
		else if (serial_port == 1) {
			UCSR1A &= ~(1<<RXC1); // clear flag -- do I need this?
		}
#endif
		sei();
	}
};

SimpleBuffer<char> Serial::tx[] = { SimpleBuffer<char>(32) };
SimpleBuffer<char> Serial::rx[] = { SimpleBuffer<char>(32) };
Output* Serial::txden[] = { nullptr };

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