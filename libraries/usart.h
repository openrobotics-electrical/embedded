/*
 * usart.h
 *
 * Created: 07/12/2011 15:16:27
 *  Author: Boomber
 */

#ifndef USART_H_
#define USART_H_

#define FOSC 16000000 // Clock Speed

unsigned int convertBaud(unsigned int baud);
void USART_Init(unsigned int ubrr);
void USART_Sendbyte( unsigned char data );
void USART_Send_string(const char *str);
void USART_Send_uint(uint32_t u);
void USART_Send_int(int16_t i);

unsigned char USART_Receive( void );


#endif /* USART_H_ */