/*
 * dataStructures.h
 *
 * Created: 1/16/2016 5:50:58 PM
 *  Author: Maxim
 */ 

#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_

#define DATA_STRUCTURE_REF &dataIn, sizeof(dataIn), &dataOut, sizeof(dataOut)

volatile struct dataIn
{
	uint8_t speed1;
	uint8_t speed2;
	uint8_t status;
} dataIn;

volatile struct dataOut
{
	char output[16];
} dataOut;

#endif /* DATASTRUCTURES_H_ */