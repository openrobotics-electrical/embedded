#ifndef SERIAL_H
#define SERIAL_H
/*
 * serial.h
 *
 * Created: 10/18/2015 8:17 PM
 * Author: Maxim
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void serial_transmit(char* s, uint8_t char_count);

volatile char* transmitting;
volatile uint8_t chars_left = 0, chars_sent = 0;

#define RX_BUFFER_SIZE 16
volatile char receiving[16];
volatile uint8_t rx_index = 0, rx_head = 0, chars_unread = 0;

ISR(USART_TX_vect) {
	
	if(chars_left-- > 0) {	
		UDR0 = transmitting[chars_sent++];	
	} else {
		chars_sent = 0;
	}
}

bool flag = false;

ISR(USART_RX_vect) {
	
	UCSR0A &= ~_BV(RXC0); // clear flag
	receiving[rx_index] = UDR0;
	rx_index = (rx_index + 1) % RX_BUFFER_SIZE;
	chars_unread++;
	PORTB = ~PORTB;
}

void serial_init() {
	
	UBRR0H = 0;
	UBRR0L = 16; // BAUD 115200
	UCSR0A = _BV(U2X0);
	UCSR0B = _BV(TXCIE0) | _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

void serial_transmit(char* s, uint8_t char_count) {
	
	// sends between 1 and 255 chars
	
	transmitting = s;
	UDR0 = s[0];
	chars_left = char_count - 1;
	chars_sent++;
}

char serial_getchar(bool* error_flag) {

	if(chars_unread > 0) {
		
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