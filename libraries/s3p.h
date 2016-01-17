#ifndef S3P_H_
#define S3P_H_
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

#define TX_PIN 1
#define TX_PORT PORTD
#define RX_PIN 0
#define RX_PORT PORTD
#define TXDEN_PIN 2
#define TXDEN_PORT PORTB

#define _S3P_TX_ENABLE()\
UCSR0B |= _BV(TXEN0);\
TXDEN_PORT |= _BV(TXDEN_PIN); /* RS485 enable */\

#define _S3P_TX_DISABLE()\
TXDEN_PORT &= ~_BV(TXDEN_PIN); /* RS485 disable */\
UCSR0B &= ~_BV(TXEN0); /* disable USART TX */\

extern char* _S3P_delimiter;
extern uint8_t _S3P_delimiterLength;

extern char* _S3P_input;
extern uint8_t _S3P_inputIndex, _S3P_inputSize;

extern char* _S3P_output;
extern uint8_t _S3P_outputIndex, _S3P_outputSize;

extern uint8_t* _S3P_memoryLocation; // points to user memory
extern uint8_t _S3P_memoryIndex;
extern uint8_t _S3P_memorySize;

extern volatile char* _S3P_transmitting;
extern volatile uint8_t _S3P_charsLeft, _S3P_charsToSend;

class S3P
{
public:	
	static void init(
			volatile void* in,
			uint8_t inSize,
			volatile void* out,
			uint8_t outSize);
	static void setbuffers(
			volatile void* in,
			uint8_t in_size,
			volatile void* out,
			uint8_t out_size);
	static void transmit(volatile void* toTransmit, uint8_t char_count);
	static void transmit();
	static void setDelimiter(const void* delimiter);
};

#endif /* S3P_h_ */