#ifndef SERIAL_H
#define SERIAL_H
/*
 * serial.h
 *
 * Created: 10/18/2015 8:17 PM
 * Author: Maxim
 */ 

/*
#if defined (__AVR_ATtiny441__)
#include <attiny441_config.h>
#elif defined (__AVR_ATmega328P__)
#include <atmega328P_config.h>
#endif
*/

void serial_transmit(char* s, uint8_t char_count);

const uint8_t TX_BUFFER_SIZE(16);
volatile char transmitting[TX_BUFFER_SIZE];
volatile uint8_t tx_index(0), tx_head(0), chars_left(0);

const uint8_t RX_BUFFER_SIZE(16);
volatile char receiving[RX_BUFFER_SIZE];
volatile uint8_t rx_index(0), rx_head(0), chars_unread(0);

ISR(USART_TX_vect) {
	cli();
	if (chars_left > 0) {	
		UDR0 = transmitting[tx_head];	
		tx_head = (tx_head + 1) % TX_BUFFER_SIZE;
		chars_left--;
	}
	sei();
}

ISR(USART_RX_vect) {	
	cli();
	UCSR0A &= ~_BV(RXC0); // clear flag
	receiving[rx_index] = UDR0;
	rx_index = (rx_index + 1) % RX_BUFFER_SIZE;
	chars_unread++;
	sei();
}

void serial_init() {
	UBRR0H = 0;
	UBRR0L = 16; // BAUD 115200
	UCSR0A = _BV(U2X0);
	UCSR0B = _BV(TXCIE0) | _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

void serial_transmit(char* s, uint8_t char_count) {
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

char serial_getchar(bool* error_flag) {
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

#endif