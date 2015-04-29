/*
 * blank.cpp
 *
 * Created: 4/29/2015 9:28:17 AM
 *  Author: Max Prokopenko
 *  max@theprogrammingclub.com
 */

#define F_CPU					16000000L
#define CHAR_BUFFER_LENGTH		20
#define COM_SPEED				57600

#include <avr/io.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h> 
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdarg.h>
#include "../../lib/usart.cpp"
#include "../../lib/io_utils.h"

//------------------VARIABLES

char strOut[CHAR_BUFFER_LENGTH];
char strIn[CHAR_BUFFER_LENGTH];

//------------------MAIN

int main(void) {
	
	USART_Init(convertBaud(COM_SPEED));
	
}
