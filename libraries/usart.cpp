/*
 * usart.cpp
 *
 * Created: 07/12/2011 15:17:35
 *  Author: Boomber
 */
#include <avr/io.h>
#include <math.h>
#include <string.h>
#include <stdio.h> // Conversions
#include "usart.h"

unsigned int convertBaud(unsigned int baud) {
	return (((((FOSC * 10) / (16L * baud)) + 5) / 10) - 1);
}

void USART_Init(unsigned int ubrr) {
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//Enable receiver and transmitter */
	UCSR0B = (1 << RXEN0)|(1 << TXEN0);
	/* Set frame format: 8data, 1 stop bit */
	UCSR0C = (0 << USBS0)|(3 << UCSZ00);
}

void USART_Sendbyte(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0))); // wait
	UDR0 = char(data);
}

void USART_Send_string(const char *str) {
	while (*str) 
		USART_Sendbyte(*str++);	
}

void USART_Send_uint(uint32_t u) {
	char str[20];
	sprintf(str, "%lu", u);
	USART_Send_string(str);
}

void USART_Send_int(int16_t i) {
	char str[10];
	sprintf(str, "%d", i);
	USART_Send_string(str);
}

unsigned char USART_Receive(void) {
	while (!(UCSR0A & (1 << RXC0))); // wait
	return UDR0;
}