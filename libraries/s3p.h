#ifndef S3P_H
#define S3P_H
/*
 * s3p.h
 *
 * Created: 11/03/2015
 * Author: Maxim
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void s3p_transmit(char* s, uint8_t char_count);

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

uint8_t data_register_in[128];
uint8_t data_register_out[128];
uint8_t address;

bool delimiter_found, address_found;
uint8_t* memory_location;
uint8_t memory_index;
uint8_t memory_size;

void s3p_send_input_to(void* memory, uint8_t size)
{
	memory_location = (uint8_t*)memory;
	memory_size = size;
};

void s3p_copy_input_buffer() {
	for(uint8_t i = 0; i < memory_size; i++) {
		memory_location[i] = data_register_in[i];
	}
}

ISR(USART_RX_vect) 
{	
	UCSR0A &= ~_BV(RXC0); // clear flag
	char received = UDR0;
	
	/*
	if(!delimiter_found) {
		delimiter_found = (received  == 128);
	} else if(!address_found ) {
		address_found = (received  == address);
	} else if(memory_index < memory_size) {
		memory_location[memory_index] = received;
	} else {
		memory_index = 0;
		delimiter_found = false;
		address_found = false;	
	}
	*/
	s3p_transmit(&received, 1);
	
	PORTB = ~PORTB;
}

void s3p_init() {
	
	UBRR0H = 0;
	UBRR0L = 16; // BAUD 115200
	UCSR0A = _BV(U2X0);
	UCSR0B = _BV(TXCIE0) | _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

void s3p_transmit(char* s, uint8_t char_count) {
	
	// sends between 1 and 255 chars
	
	transmitting = s;
	UDR0 = s[0];
	chars_left = char_count - 1;
	chars_sent++;
}

#endif