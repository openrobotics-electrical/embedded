#ifndef OPBOTS_H
#define OPBOTS_H

#define F_CPU 16000000
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

namespace opbots {
	
void (*_error_function)(void) = nullptr; // Set by user in app to point to shutdown function
void (*_error_led_function)(void) = nullptr; // Set by user in app to flash error light

enum ErrorType {
	NULL_POINTER=1,
	OUT_OF_MEMORY=2,
	BUFFER_OVERFLOW_TX=3,
	BUFFER_OVERFLOW_RX=4,
	OVER_VOLTAGE=5,
	OVER_CURRENT=6,
	TEST=7
};

void _raise_error(ErrorType error) {
	cli();
	uint16_t delay_time(100);
	if (_error_function != nullptr && _error_led_function != nullptr) {
		// Call user-defined error function
		_error_function();	
		// Flash error code on err_led forever
		while (1) {
			for (int i=0; i < uint8_t(error); ++i) {
				_error_led_function();
				_delay_ms(delay_time);
				_error_led_function();
				_delay_ms(5*delay_time);
			}
			_delay_ms(10*delay_time);
		}
	}
	sei();
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
	static uint16_t immediate_read(uint8_t n) {
		Analog::select_channel(n);
		Analog::start_conversion();
		while (!Analog::conversion_complete()) {
			// idle
		}
		return Analog::get_value();
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
	// Scales reading from adc to range from 0-SCALE, calculating 100ths as well
	// E.g. you have a sensor that has a range of 0 to 2 volts, each volt corresponds to 10 pascals of pressure,
	// Therefore SCALE argument is 50 as the whole ADC range from 0 to 5 volts equates to 0 to 50 pascals
	template<uint8_t SCALE>
	static void inline adc_scale(const uint16_t adc_reading, uint16_t *result, uint16_t *result_centis=nullptr) {
		uint32_t temp_result(adc_reading);
		if (result == nullptr) {
			_raise_error(ErrorType::NULL_POINTER);
		}
		temp_result *= (SCALE*0x10000 / 0x400);
		if (result_centis != nullptr) {
			*result_centis = uint16_t(((temp_result % 0x10000) * 100) >> 16);
		}
		temp_result = temp_result >> 16;
		*result = uint16_t(temp_result);
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
	
	SimpleBuffer(const uint8_t buffer_size) : size(buffer_size), head(0), tail(0) {
		buffer = (T*)malloc(size);
	}
	
	~SimpleBuffer() {
		free(buffer);
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
		if (serial_port == 1) offset = 0x10;
		*(&UBRR0H+offset) = baud / 0xFF;
		*(&UBRR0L+offset) = baud % 0xFF;
		*(&UCSR0A+offset) = 1<<(U2X0);
		*(&UCSR0B+offset) = 1<<(TXCIE0) | 1<<(RXCIE0) | 1<<(RXEN0) | 1<<(TXEN0);
		*(&UCSR0C+offset) = 1<<(UCSZ01) | 1<<(UCSZ00);
	}
	template<typename T>
	static void transmit(T* buffer, uint8_t char_count, const uint8_t serial_port=0) {
		if (char_count > 0) {
			// sends between 1 and 255 chars if large enough buffer allocated
			// does not prevent against buffer overwrites, increase
			// buffer size if needed
			if (txden[serial_port] != nullptr) {
				txden[serial_port]->set();
				UCSR0B = 1<<(TXCIE0) | 1<<(TXEN0);
			}
			if (serial_port == 0) {
				if (char_count > tx[0].size) {
					_raise_error(ErrorType::BUFFER_OVERFLOW_TX);
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
		}
	}
	static bool transmission_complete(const uint8_t serial_port=0) {
		cli();
		const bool ret(tx[serial_port].head >= tx[serial_port].tail);
		sei();
		return ret;
	}
	static void set_txden_pin(Output& txden_pin, const uint8_t serial_port=0) {
		Serial::txden[serial_port] = &txden_pin;
	}
	static bool empty() {
		return Serial::rx[0].head == 0;
	} 
	static char get_char(bool& ok) {
		if(!Serial::rx[0].head == 0) {
			rx[0].head--;
			ok = true;
			return rx[0].buffer[rx[0].head];
		} else {
			ok = false;
			return '\0';
		}
	}
	static inline void service_tx_interrupt(const uint8_t serial_port=0) {
		cli();
		tx[serial_port].head++;
		if (tx[serial_port].head < tx[serial_port].tail) {
			if (serial_port == 0) {
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
				UCSR0B = 1<<(RXCIE0) | 1<<(RXEN0);
			}
		}
		sei();
	}
	static inline void service_rx_interrupt(const uint8_t serial_port=0) {
		cli();
		PORTB ^= 1<<4; // MISO
		if (serial_port == 0) {
			UCSR0A &= ~(1<<RXC0); // clear flag -- do I need this?
			if (rx[serial_port].head < rx[serial_port].size) {
				rx[serial_port].buffer[rx[serial_port].head] = UDR0;
				rx[serial_port].head++;
			} else {
				_raise_error(ErrorType::BUFFER_OVERFLOW_RX);
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

/*
namespace timers {
	
	enum Prescaler {
		PRESCALER_1 = 1U,
		PRESCALER_8 = 2U,
		PRESCALER_64 = 3U,
		PRESCALER_256 = 4U,
		PRESCALER_1024 = 5U,
		PRESCALER_MASK = 7U
	};

	class GenericTimer {
		protected:
		Prescaler prescale;
		GenericTimer(Prescaler pre) : prescale(pre) {}
		public:
		virtual void inline start() = 0;
		virtual void inline stop() = 0;
		virtual bool inline is_running() = 0;
		virtual void inline clear() = 0;
	};

	class Timer8bit : public GenericTimer {
		protected:
		Timer8bit(Prescaler pre) : GenericTimer(pre) {}
		
		public:
		virtual uint8_t inline count() = 0;
		
		void inline start() { TCCR0B |= prescale; }
		void inline stop() { TCCR0B &= ~PRESCALER_MASK; }
		bool inline is_running() { return TCCR0B &= ~PRESCALER_MASK; }
		void inline set_output_compare_A(uint8_t oc_A) { OCR0A = oc_A; };
		void inline set_output_compare_B(uint8_t oc_B) { OCR0B = oc_B; };
	};

	class Timer16bit : public GenericTimer {
		protected:
		Timer16bit(Prescaler pre, volatile uint8_t *const base)
		: GenericTimer(pre), base_address(base) {}
		
		volatile uint8_t *const base_address; // Child classes must initialize this

		public:
		virtual uint16_t count() = 0;

		void inline start() { *(base_address-1) |= prescale; }
		void inline stop() { *(base_address-1) &= ~PRESCALER_MASK; }
		bool inline is_running() { return *(base_address-1) & PRESCALER_MASK > 0; }
		// TODO implement
		void inline set_output_compare_A(uint16_t oc_A) { *(base_address-6) = oc_A; };
		// TODO implement
		void inline set_output_compare_B(uint16_t oc_B) { *(base_address-8) = oc_B; };
	};

	class Timer0 : Timer8bit {
		public:
		Timer0(Prescaler pre) : Timer8bit(pre) {}
		uint8_t count() { return TCNT0; }
		void clear() { TCNT0 = 0; }
	};

	class Timer1 : public Timer16bit {
		public:
		Timer1(Prescaler pre) : Timer16bit(pre, &TCCR1A) {
			TCCR1A = (1<<COM1B1) | (1<<COM1A1) | (1<<WGM11);
			TCCR1B = (1<<WGM13) | (1<<WGM12);
		}
		uint16_t count() { return TCNT1; }
		void clear() { TCNT1 = 0; }
	};

	class Timer2 : public Timer16bit {
		public:
		Timer2(Prescaler pre) : Timer16bit(pre, &TCCR2A) {
			TCCR2A = (1<<COM1B1) | (1<<COM1A1) | (1<<WGM11);
			TCCR2B = (1<<WGM13) | (1<<WGM12);
		}
		uint16_t count() { return TCNT2; }
		void clear() { TCNT2 = 0; }
	};
}
*/

enum Prescaler1 {
	PRESCALER1_1 = 1U,
	PRESCALER1_8 = 2U,
	PRESCALER1_64 = 3U,
	PRESCALER1_256 = 4U,
	PRESCALER1_1024 = 5U,
	PRESCALER1_MASK = 7U
};

enum Prescaler2 {
	PRESCALER2_1 = 0x01,
	PRESCALER2_8 = 0x02,
	PRESCALER2_32 = 0x03,
	PRESCALER2_64 = 0x04,
	PRESCALE2R_128 = 0x05,
	PRESCALER2_256 = 0x06,
	PRESCALER2_1024 = 0x07,
	PRESCALER2_MASK = 0x07
};

class Timer1 {
private:
	Prescaler1 prescale;
public:
	Timer1(Prescaler1 pre) : prescale(pre) {
		TCCR1A = (1<<COM1B1) | (1<<COM1A1) | (1<<WGM11);
		TCCR1B = (1<<WGM13) | (1<<WGM12);
	}
	void inline start() { TCCR1B |= prescale; }
	void inline stop() { TCCR1B &= ~PRESCALER1_MASK; }
	bool is_running() { return TCCR1B & PRESCALER1_MASK; }
	uint16_t count() { return TCNT1; }
	inline void clear() { TCNT1 = 0; }
};

SimpleBuffer<char> Serial::tx[] = { SimpleBuffer<char>(128) };
SimpleBuffer<char> Serial::rx[] = { SimpleBuffer<char>(64) };
Output* Serial::txden[] = { nullptr };

} /* end of namespace opbots */

using namespace opbots;

/* Basic allocators for classes */
void * operator new(size_t n) {
	void * const p = malloc(n);
	if (p == nullptr) {
		_raise_error(ErrorType::OUT_OF_MEMORY);
	}
	return p;
}

void operator delete(void * p) {
	free(p);
}

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