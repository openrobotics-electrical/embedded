/*
 * dataStructures.h
 *
 * Created: 1/16/2016 5:50:58 PM
 *  Author: Maxim
 */ 

#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_

#define REF(x) &x, sizeof(x)
#define DATA_STRUCTURE_REF &_delimiter, sizeof(_delimiter) - 1, REF(dataIn), REF(dataOut)
#define ATOMIC(x) cli();x;sei()

const char _delimiter[] = "@MARM";

volatile struct dataIn
{
	uint8_t status;
} dataIn;

volatile struct dataOut
{
	uint16_t voltage[8];
} dataOut;

#endif /* DATASTRUCTURES_H_ */