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

bool delimiter_found, address_found;
uint8_t* memory_location;
uint8_t memory_index;
uint8_t memory_size;

volatile char* transmitting;
volatile uint8_t chars_left = 0, chars_to_send = 0;
	
#define TX_PIN 1
#define TX_PORT PORTD
#define TXDEN_PIN 3
#define TXDEN_PORT PORTB

void s3p_TX_enable() 
{ 
	UCSR0B |= _BV(TXEN0); // TX pin enabled
	TXDEN_PORT |= _BV(TXDEN_PIN);
}

void s3p_TX_disable() 
{ 	
	TXDEN_PORT &= ~TXDEN_PIN;
	UCSR0B &= ~_BV(TXEN0); // TX pin disabled
	TX_PORT |= _BV(TX_PIN);
}

ISR(USART_TX_vect)
{
	s3p_TX_disable();
	UCSR0B &= ~_BV(TXCIE0); // disables TX complete interrupt
}

ISR(USART_RX_vect) 
{	
	char received = UDR0; // clears flag
	
	char delimiter[] = "@";
	
	if(memory_index < sizeof(delimiter)) 
	{
		memory_index = (received == delimiter[memory_index])? memory_index + 1 : 0;	
	} 
	else
	{
		
	}
}

ISR(USART_UDRE_vect)
{
	UDR0 = transmitting[chars_to_send - chars_left];
	chars_left--;
	
	if(chars_left == 1) 
	{
		UCSR0B &= ~_BV(UDRIE0);  // disable buffer empty interrupt
		UCSR0B |= _BV(TXCIE0); // enables TX complete interrupt
	}
}

void s3p_init() {
	
	DDRC = 0xff;
	DDRB = 0xff;
	
	PORTD |= _BV(1);
	
	UBRR0H = 0;
	UBRR0L = 1; // 1Mbaud
	UCSR0A = _BV(U2X0);
	UCSR0B = /*_BV(RXCIE0) |*/ _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

void s3p_transmit(char* s, uint8_t char_count) {
	
	// sends between 1 and 255 chars
	// uses USART_TX and USART_UDRE interrupts to advance through chars
		
	transmitting = s;
	chars_to_send = char_count;
	chars_left = chars_to_send - 1;
	
	s3p_TX_enable();
	UDR0 = s[0]; // start transmission of first char
	UCSR0B |= _BV(UDRIE0); // enable buffer empty interrupt
	
	TX_PORT = _BV(TX_PIN);
}

#endif