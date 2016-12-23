/*
 * atmega328P template
 * Created: 12/22/2016 2:42:08 AM
 * Author : Maxim
 */ 

#include "opbots.h"
#include <stdio.h>
using namespace opbots;

const uint8_t ERR(0);

Output err_led(&PORTB, PIN0);
Output enable[] = {
	Output(&PORTB, PIN0),
	Output(&PORTC, PIN3),
	Output(&PORTD, PIN2),
	Output(&PORTD, PIN4),
	Output(&PORTC, PIN0),
	Output(&PORTC, PIN1),
	Output(&PORTC, PIN2)
};
// Used by 10-bit resolution ADC
Input current_pin(&PORTC, PIN7); // 0.2 V per amp
Input voltage_pin(&PORTC, PIN6); // 0.2 V per volt

const uint8_t delay_time(100); // ms interval base for flashing lights

void startup_routine(int loops=1) {	
	while (--loops >= 0) {
		for (Output out : enable) {
			out.set();
			_delay_ms(delay_time/2);
			out.clear();
		}
	}
}

void raise_error(ErrorType error) {
	cli(); // Disable all interrupts
	// Disable all power outputs
	for (int i=1; i<=6; ++i) enable[i].clear();
	// Flash error code on err_led
	while (1) {
		for (int i=0; i < uint8_t(error); ++i) {
			err_led.set();
			_delay_ms(delay_time);
			err_led.clear();
			_delay_ms(5*delay_time);
		}
		_delay_ms(10*delay_time);
	}
}

template<uint8_t SCALE>
void inline adc_scale(uint16_t adc_reading, uint16_t *result, uint16_t *result_centis=nullptr) {
	uint32_t temp_result(adc_reading);
	if (result == nullptr) {
		raise_error(ErrorType::NULL_POINTER);
	}
	temp_result *= (SCALE*0x10000 / 0x400);
	if (result_centis != nullptr) {
		*result_centis = uint16_t(((temp_result % 0x10000) * 100) >> 16);
	}
	temp_result = temp_result >> 16;
	*result = uint16_t(temp_result);
}

const uint8_t MAX_ADC_UNITS(25);
char message[16];
uint16_t volts_reading, volts, centivolts;
uint16_t amps_reading, amps, centiamps;
uint32_t watts_raw, watts, centiwatts;

int main(void) {
	startup_routine(2);
	
	Analog::selectChannel(PIN7);
	Serial::init();
	sei(); // Enable interrupts
	
    while (1) {
		for (int i=1; i<=6; ++i) enable[i].toggle();
		
		Analog::selectChannel(PIN6);
		Analog::startConversion();
		while (!Analog::conversionComplete()) { /* idle */ }
		volts_reading = Analog::getValue();
		adc_scale<MAX_ADC_UNITS>(volts_reading, &amps, &centiamps);
		sprintf(message, "%2u.%02u A\r\n", amps, centiamps);
		Serial::transmit(message, 10);
		_delay_ms(500);
		
		Analog::selectChannel(PIN7);
		Analog::startConversion();
		while (!Analog::conversionComplete()) { /* idle */ }
		amps_reading = Analog::getValue();
		adc_scale<MAX_ADC_UNITS>(amps_reading, &volts, &centivolts);
		sprintf(message, "%2u.%02u V\r\n", volts, centivolts);
		Serial::transmit(message, 10);
		_delay_ms(500);
		
		watts_raw = uint32_t(volts_reading) * uint32_t(amps_reading);
		watts = (watts_raw * 39) >> 16;
		centiwatts = ((watts_raw % (watts * 1680)) * 3901) >> 16;
		sprintf(message, "%2lu.%02lu W\r\n", watts, centiwatts);
		Serial::transmit(message, 10);
		_delay_ms(500);
    }
}

