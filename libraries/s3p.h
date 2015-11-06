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

uint8_t data_register_in[128];
uint8_t data_register_out[128];
uint8_t address;

bool delimiter_found, address_found;
uint8_t* memory_location;
uint8_t memory_index;
uint8_t memory_size;

volatile char* transmitting;
volatile uint8_t chars_left = 0, chars_to_send = 0;

#define RX_BUFFER_SIZE 16
volatile char receiving[16];
volatile uint8_t rx_index = 0, rx_head = 0, chars_unread = 0;

void s3p_buffer_empty_interrupt_enable()
{
	UCSR0B |= _BV(UDRIE0);
}

void s3p_buffer_empty_interrupt_disable()
{
	UCSR0B &= ~_BV(UDRIE0);
}

void s3p_tx_complete_interrupt_enable()
{
	UCSR0B |= _BV(TXCIE0);
}

void s3p_tx_complete_interrupt_disable()
{
	UCSR0B &= ~_BV(TXCIE0);
}

void s3p_TX_enable() 
{ 
	PORTB = 1; 
	UCSR0B |= _BV(TXEN0);
}

void s3p_TX_disable() 
{ 	
	PORTB = 0; 
	UCSR0B &= ~_BV(TXEN0);
}

ISR(USART_TX_vect)
{
	PORTC = 2;
	
	s3p_TX_disable();
	s3p_tx_complete_interrupt_disable();
	
	PORTC = 0;
}

/*
ISR(USART_TX_vect) 
{
	UDR0 = transmitting[chars_sent];
		
	if(chars_left == 1) {
		s3p_TX_disable();
		chars_sent = 0;
	}
	
	chars_sent++;	
	chars_left--;
}
*/

ISR(USART_RX_vect) 
{	
	PORTC = 2;
	
	char received = UDR0; // clears flag
	
	char delimiter[] = "@";
	
	if(memory_index < sizeof(delimiter)) 
	{
		memory_index = (received == delimiter[memory_index])? memory_index + 1 : 0;	
	} 
	else
	{
		
	}

	PORTC = 0;
}

ISR(USART_UDRE_vect)
{
	PORTC = 1;
	
	// s3p_buffer_empty_interrupt_disable();
	UDR0 = transmitting[chars_to_send - chars_left];
	chars_left--;
	
	if(chars_left == 1) 
	{
		s3p_buffer_empty_interrupt_disable();
		s3p_tx_complete_interrupt_enable();
	}
	
	PORTC = 0;
}

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

void s3p_init() {
	
	DDRC = 0xff;
	DDRB = 0xff;
	
	PORTD |= _BV(1);
	
	UBRR0H = 0;
	UBRR0L = 1;
	UCSR0A = _BV(U2X0);
	UCSR0B = /*_BV(RXCIE0) |*/ _BV(RXEN0) | _BV(TXEN0);
	// UCSR0B |= _BV(TXCIE0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

void s3p_transmit(char* s, uint8_t char_count) {
	
	// sends between 1 and 255 chars
	// uses USART_TX and USART_UDRE interrupts to advance through chars
	
	PORTD = _BV(6) | _BV(1);
	
	transmitting = s;
	chars_to_send = char_count;
	chars_left = chars_to_send - 1;
	
	s3p_TX_enable();
	UDR0 = s[0];
	s3p_buffer_empty_interrupt_enable();
	
	PORTD = _BV(1);
}

#endif