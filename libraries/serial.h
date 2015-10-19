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

volatile char* transmitting;
volatile uint8_t chars_left = 0, chars_sent = 0;

ISR(USART_TX_vect) {
	
	if(chars_left-- > 0) {
		
		UDR0 = transmitting[chars_sent++];
		
		} else {
		
		chars_sent = 0;
	}
}

void serial_init() {
	
	UBRR0H = 0;
	UBRR0L = 16; // BAUD 115200
	UCSR0A = _BV(U2X0);
	UCSR0B = _BV(TXCIE0) | _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

void serial_transmit(char* s, uint8_t char_count) {
	
	// sends between 1 and 255 chars
	
	transmitting = s;
	UDR0 = s[0];
	chars_left = char_count - 1;
	chars_sent++;
}

#endif