/*
 * S3P.cpp
 *
 * Created: 1/16/2016 6:06:35 PM
 *  Author: Maxim
 */ 

#include <S3P.h>

#define DON //PORTB |= _BV(5)
#define DOFF //PORTB &= ~_BV(5)

uint8_t* _S3P_delimiter;
uint8_t _S3P_delimiterSize;

volatile uint8_t* _S3P_input;
volatile uint8_t _S3P_inputIndex = 0;
uint8_t _S3P_inputSize;

volatile uint8_t* _S3P_output;
volatile uint8_t _S3P_outputIndex = 0;
uint8_t _S3P_outputSize;

volatile uint8_t* _S3P_memoryLocation;
volatile uint8_t _S3P_memoryIndex = 0;
uint8_t _S3P_memorySize;

volatile uint8_t* _S3P_transmitting;
volatile uint8_t _S3P_charsLeft = 0, _S3P_charsToSend = 0;

ISR(USART_TX_vect)
{
	DON;
	UCSR0B &= ~_BV(TXCIE0); // disables TX complete interrupt
	_S3P_TX_DISABLE();
	DOFF;
}

ISR(USART_RX_vect)
{
	DON;
	char received = UDR0; // clears flag
	
	if(_S3P_memoryIndex < _S3P_delimiterSize)
	{
		_S3P_memoryIndex = (received == _S3P_delimiter[_S3P_memoryIndex])? 
			_S3P_memoryIndex + 1 : 0;
	}
	else if(_S3P_inputIndex < _S3P_inputSize)
	{
		_S3P_input[_S3P_inputIndex] = received;
		_S3P_inputIndex++;
	}
	if(_S3P_inputIndex == _S3P_inputSize)
	{
		_S3P_memoryIndex = 0;
		_S3P_inputIndex = 0;
		S3P::transmit();
	}
	DOFF;
}

ISR(USART_UDRE_vect)
{
	DON;
	UDR0 = _S3P_transmitting[_S3P_charsToSend - _S3P_charsLeft];
	_S3P_charsLeft--;
	
	if(_S3P_charsLeft == 0)
	{
		UCSR0B &= ~_BV(UDRIE0);  // disable buffer empty interrupt
		UCSR0B |= _BV(TXCIE0); // enables TX complete interrupt
	}
	DOFF;
}

void S3P::init(
		const void* delimiter,
		uint8_t delimiterSize,
		volatile void* in,
		uint8_t inSize,
		volatile void* out,
		uint8_t outSize)
{	
	setDelimiter(delimiter, delimiterSize);
	setbuffers(in, inSize, out, outSize);
	
	DDRB |= _BV(TXDEN_PIN) + _BV(5);
	PORTB &= ~_BV(TXDEN_PIN);
	
	UBRR0H = 0;
	UBRR0L = 7; // 250000 baud
	UCSR0A = _BV(U2X0); // Double speed UART
	UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0); // Receive interrupt, RX/TX enable
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // 8-bit data, no parity, 1 stop bit
}

void S3P::transmit(volatile void* toTransmit, uint8_t charCount) 
{
	DON;
	// sends between 1 and 255 chars
	// uses USART_TX and USART_UDRE interrupts to advance through chars
	
	_S3P_transmitting = (volatile uint8_t*)toTransmit;
	_S3P_charsToSend = charCount;
	_S3P_charsLeft = _S3P_charsToSend - 1;
	
	_S3P_TX_ENABLE();
	
	UDR0 = _S3P_transmitting[0]; // start transmission of first char
	
	if (_S3P_charsLeft > 0)
	{
		UCSR0B |= _BV(UDRIE0); // enable buffer empty interrupt
	}
	else
	{
		UCSR0B |= _BV(TXCIE0); // enables TX complete interrupt
	}
	DOFF;
}

void S3P::transmit()
{
	transmit(_S3P_output, _S3P_outputSize);
}

void S3P::setbuffers(
		volatile void* in,
		uint8_t inSize,
		volatile void* out,
		uint8_t outSize)
{
	_S3P_input = (volatile uint8_t*)in;
	_S3P_inputSize = inSize;
	_S3P_output = (volatile uint8_t*)out;
	_S3P_outputSize = outSize;
}

void S3P::setDelimiter(const void* delimiter, uint8_t delimiterSize)
{
	_S3P_delimiter = (uint8_t*)delimiter;
	_S3P_delimiterSize = delimiterSize;
}