/*
 * atmega328P template
 * Created: 12/22/2016 2:42:08 AM
 * Author : Maxim
 */ 

#include "opbots.h"
#include <stdio.h>
using namespace opbots;

Output enable[] = {
	Output(&PORTB,0),
	Output(&PORTC,3),
	Output(&PORTD,2),
	Output(&PORTD,4),
	Output(&PORTC,0),
	Output(&PORTC,1),
	Output(&PORTC,2)
};
Output& err_led(enable[0]);
Output txden(&PORTD,7);
Output sck(&PORTB,5);
Output miso(&PORTB,4);
Output mosi(&PORTB,3);
Output pwm0a(&PORTD,6);
Input reset(&PORTC,6);

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

template<uint8_t SCALE>
void inline adc_scale(uint16_t adc_reading, uint16_t *result, uint16_t *result_centis=nullptr) {
	uint32_t temp_result(adc_reading);
	if (result == nullptr) {
		_raise_error(ErrorType::NULL_POINTER);
	}
	temp_result *= (SCALE*0x10000 / 0x400);
	if (result_centis != nullptr) {
		*result_centis = uint16_t(((temp_result % 0x10000) * 100) >> 16);
	}
	temp_result = temp_result >> 16;
	*result = uint16_t(temp_result);
}

const uint8_t MAX_VOLTS(25);
const uint8_t MAX_AMPS(25);
char message[16];
uint16_t volts_reading, volts, centivolts;
uint16_t amps_reading, amps, centiamps;
uint32_t watts_raw, watts, centiwatts;  

int main(void) {
	startup_routine(2);
	     
	Analog::select_channel(7);
	Serial::set_txden_pin(txden);
	Serial::init(115200);
	err_led.clear();
	uint16_t pulse_width(1500);
	sei(); // Enable interrupts
	
    while (1) {	
		bool ok;
		cli();
		char c(Serial::get_char(ok));
		sei();
		if (ok) {
			switch(c) {
				case '1': enable[1].toggle(); break;
				case '2': enable[2].toggle(); break;
				case '3': enable[3].toggle(); break;
				case '4': enable[4].toggle(); break;
				case '5': enable[5].toggle(); break;
				case '6': enable[6].toggle(); break;
				case ',': 
					pulse_width += 10; 
					sprintf(message,"Pulse width %u\r\n",pulse_width);
					break;
				case '.': 
					pulse_width -= 10; 
					sprintf(message,"Pulse width %u\r\n",pulse_width);
					break;
				case 'a':
					volts_reading = Analog::immediate_read(6);
					adc_scale<MAX_VOLTS>(volts_reading, &amps, &centiamps);
					sprintf(message, "%2u.%02u A  \r\n", amps, centiamps);
					break;
				case 'v':
					amps_reading = Analog::immediate_read(7);
					adc_scale<MAX_AMPS>(amps_reading, &volts, &centivolts);
					sprintf(message, "%2u.%02u V  \r\n", volts, centivolts);
					break;
				case 'w':
					volts_reading = Analog::immediate_read(6);
					amps_reading = Analog::immediate_read(7);
					watts_raw = uint32_t(volts_reading) * uint32_t(amps_reading);
					watts = (watts_raw * 39) >> 16;
					centiwatts = ((watts_raw % (watts * 1680)) * 3901) >> 16;
					sprintf(message, "%2lu.%02lu W  \r\n", watts, centiwatts);
					break;
				case 'e':
					sprintf(message,"max@embeddedprofessional.com\r\n");
					break;
				default:
					sprintf(message,"Bad command\r\n");
			}
			Serial::transmit(message,strlen(message));
		}
		pwm0a.set();
		uint16_t m(pulse_width/10);
		for (uint16_t i(0); i < m; ++i)
			_delay_us(9);
		pwm0a.clear();
		_delay_ms(20);
    }
}

void toggle_lights() {
	for (int i=1; i<=6; ++i) enable[i].toggle();
}

