#ifndef S3P_H
#define S3P_H
/*
 * s3p.h
 *
 * Created: 11/03/2015
 * Author: Maxim
 */ 

/*

S3P = SSSP = (S)uper-(S)imple (S)erial (P)rotocol

This is an in-progress communication protocol
Uses interrupts to transmit and receive at up to 1 Mbps
Checks received data against a preset delimiter
Right now just echoes first three chars after delimiter
Will in future take user input/output memory blocks as arguments

*/

#include <modular8.h>
#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t* memory_location; // points to user memory
uint8_t memory_index;
uint8_t memory_size;

volatile char* transmitting;
volatile uint8_t chars_left = 0, chars_to_send = 0;
	
void s3p_init();
void s3p_setbuffers(
		volatile void* in,
		uint8_t in_size,
		volatile void* out,
		uint8_t out_size
	);
void s3p_transmit(volatile void* s, uint8_t char_count);
	
#define TX_PIN 1
#define TX_PORT PORTD
#define RX_PIN 0
#define RX_PORT PORTD
#define TXDEN_PIN 2
#define TXDEN_PORT PORTB

#define s3p_TX_enable()\
	UCSR0B |= _BV(TXEN0);\
	TXDEN_PORT |= _BV(TXDEN_PIN); /* RS485 enable */\

#define s3p_TX_disable()\
	TXDEN_PORT &= ~_BV(TXDEN_PIN); /* RS485 disable */\
	UCSR0B &= ~_BV(TXEN0); /* enable USART TX */\

ISR(USART_TX_vect)
{
	s3p_TX_disable();
	UCSR0B &= ~_BV(TXCIE0); // disables TX complete interrupt
}

char delimiter[] = "@RGB";
uint8_t delimiter_length = sizeof(delimiter) - 1;

char* input;
uint8_t input_index, input_size;

char* output;
uint8_t output_index, output_size;

ISR(USART_RX_vect) 
{	
	char received = UDR0; // clears flag
	
	if(memory_index < delimiter_length) 
	{
		memory_index = (received == delimiter[memory_index])? memory_index + 1 : 0;	
	} 
	else if(input_index < input_size)
	{
		input[input_index] = received;
		input_index++;
 	}
	if(input_index == input_size)
	{
		memory_index = 0;
		input_index = 0;
		s3p_transmit(output, output_size);
	}
}

ISR(USART_UDRE_vect)
{
	UDR0 = transmitting[chars_to_send - chars_left];
	chars_left--;
	
	if(chars_left == 0) 
	{
		UCSR0B &= ~_BV(UDRIE0);  // disable buffer empty interrupt
		UCSR0B |= _BV(TXCIE0); // enables TX complete interrupt
	}
}

void s3p_init() {
	
	DDRB |= _BV(TXDEN_PIN) + _BV(5);
	PORTB &= ~_BV(TXDEN_PIN);
	
	UBRR0H = 0; 
	UBRR0L = 7; // 250000 baud
	UCSR0A = _BV(U2X0); // double speed UART
	UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0); // receive interrupt, RX/TX enable
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // 8-bit data, no parity, 1 stop bit
}

void s3p_transmit(volatile void* s, uint8_t char_count) {
	
	// sends between 1 and 255 chars
	// uses USART_TX and USART_UDRE interrupts to advance through chars
		
	transmitting = (char*)s;
	chars_to_send = char_count;
	chars_left = chars_to_send - 1;
	
	s3p_TX_enable();
	UDR0 = transmitting[0]; // start transmission of first char
	if (chars_left > 0)
	{
		UCSR0B |= _BV(UDRIE0); // enable buffer empty interrupt
	}
	else
	{
		UCSR0B |= _BV(TXCIE0); // enables TX complete interrupt
	}
}

void s3p_setbuffers(
		volatile void* in,
		uint8_t in_size,
		volatile void* out,
		uint8_t out_size
	)
{
	input = (char*)in;
	input_size = in_size;
	output = (char*)out;
	output_size = out_size;
}

#endif