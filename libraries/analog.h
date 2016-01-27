#ifndef ANALOG_H_
#define ANALOG_H_

#include <avr/io.h>
#include <avr/interrupt.h>

class Analog {
	
public:
	volatile static bool newValue;
	volatile static uint8_t analogHigh;
	volatile static uint8_t analogLow;
	volatile static uint8_t lastChannel;
	
	static void selectChannel (uint8_t n);
	static void autoTriggerEnable(bool enabled);
	static void startConversion ();
	static void stopConversion ();
	static bool conversionComplete();
	static bool newValueAvailable();
	static uint16_t getValue ();
};

#endif /* ANALOG_H_ */
