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

void error_function() {
	for (int i=1; i<=6; ++i) enable[i].clear();
}
void error_led_function() {
	err_led.toggle();
}

const uint8_t MAX_VOLTS(25);
const uint8_t MAX_AMPS(25);
char message[16];
uint16_t volts_reading, volts, centivolts;
uint16_t amps_reading, amps, centiamps;
uint32_t watts_raw, watts, centiwatts;  

int main(void) {
	_error_function = error_function;
	_error_led_function = error_led_function;
	err_led.clear();
	startup_routine(2);
	     
	Serial::set_txden_pin(txden);
	Serial::init(115200);
	uint16_t pulse_width(1500);
	sei(); // Enable interrupts
	
	const char name[] = "@pdu";
	uint8_t stage(0);
	bool char_available;
    while (1) {	
		cli();
		char c(Serial::get_char(char_available));
		sei();
		if (char_available) {
			if (stage == 4) {
				switch(c) {
					case '1':
						enable[1].toggle();
						sprintf(message,"Enable 1 toggled\r\n");
						break;
					case '2':
						enable[2].toggle();
						sprintf(message,"Enable 2 toggled\r\n");
						break;
					case '3':
						enable[3].toggle();
						sprintf(message,"Enable 3 toggled\r\n");
						break;
					case '4':
						enable[4].toggle();
						sprintf(message,"Enable 4 toggled\r\n");
						break;
					case '5':
						enable[5].toggle();
						sprintf(message,"Enable 5 toggled\r\n");
						break;
					case '6':
						enable[6].toggle();
						sprintf(message,"Enable 6 toggled\r\n");
						break;
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
						Analog::adc_scale<MAX_VOLTS>(volts_reading, &amps, &centiamps);
						sprintf(message, "%2u.%02u A  \r\n", amps, centiamps);
						break;
					case 'v':
						amps_reading = Analog::immediate_read(7);
						Analog::adc_scale<MAX_AMPS>(amps_reading, &volts, &centivolts);
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
						_raise_error(ErrorType::OUT_OF_MEMORY);
						break;
					default:
						sprintf(message,"Bad command\r\n");
				}
				Serial::transmit(message,strlen(message));
				stage = 0;
			} else {
				if (c == name[stage]) 
					stage++;
				else
					stage = 0;
			}
		}
		/*
		pwm0a.set();
		uint16_t m(pulse_width/10);
		for (uint16_t i(0); i < m; ++i)
			_delay_us(9);
		pwm0a.clear();
		*/
		//_delay_ms(20);
    }
}

