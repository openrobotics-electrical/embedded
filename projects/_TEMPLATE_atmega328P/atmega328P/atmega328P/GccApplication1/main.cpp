/*
 * atmega328P template
 * Created: 12/22/2016 2:42:08 AM
 * Author : Maxim
 */ 

#include "opbots.h"
#include <stdio.h>
using namespace opbots;

// Setup power enable pins and error led
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

// Communication and debug signals
Output txden(&PORTD,7);
Input  rx(&PORTD,0);
Output tx(&PORTD,1);
Output sck(&PORTB,5);
Output miso(&PORTB,4);
Output mosi(&PORTB,3); // Change to input for SPI 

// PWM/general outputs
Output pwm0a(&PORTD,6);
Output pwm0b(&PORTD,5);
Output pwm1a(&PORTB,1);
Output pwm1b(&PORTB,2);
Output pwm2a(&PORTB,3);
Output pwm2b(&PORTD,3);

// Reset pin
Input  reset(&PORTC,6);

//Timer1 timer1(Prescaler::PRESCALER_8);

// Flashes err light and power outputs on briefly
void startup_routine(int loops=1) {	
	while (--loops >= 0) {
		for (Output out : enable) {
			out.set();
			_delay_ms(50);
			out.clear();
		}
	}
}

// Turns all power outputs off, pass this function to default _error_function
void error_function() {
	for (int i=1; i<=6; ++i) enable[i].clear();
}
// Toggles enable[0] corresponding to error led, pass to _error_led_function
void error_led_function() {
	enable[0].toggle();
}

const uint8_t MAX_VOLTS(25);
const uint8_t MAX_AMPS(25);
char message[16];
uint16_t volts_reading, volts, centivolts;
uint16_t amps_reading, amps, centiamps;
uint32_t watts_raw, watts, centiwatts;  

const char name[] = "@pdu";

enum State {
	NORMAL,
	IMMEDIATE,
	DRUM,
	__States_sz__
};

struct PduData {
	State current_state;
	uint16_t pulse_width;
};
volatile PduData pduData = {State::NORMAL, 1500};

void poll_serial_and_process();
void state_normal() {
	
}
void state_immediate() {
	
}
void state_drum() {
	const uint8_t vel[] = {
		2, 3,2,3,
		10,1,2,3,
		1, 3,2,3,
		10,3,2,3
	};
	for (int i(0); i < 16; ++i) {
		for (int j(0); j < 10; ++j) {
			if (vel[i] > j) {
				enable[1].set();
			} else {
				enable[1].clear();
			}
			poll_serial_and_process();
			_delay_ms(10);
		}
	}
}

void (*state_machine[__States_sz__])(void) = {
	state_normal, 
	state_immediate, 
	state_drum
};

inline void process(char c) {
	enable[6].toggle();
	static uint8_t stage(0);
	if (stage == 4 || pduData.current_state == State::IMMEDIATE) {
		switch(c) {
			case 'd':
				pduData.current_state = State::DRUM;
				sprintf(message, "Drum mode on\r\n");
				break;
			case 'i':
				pduData.current_state = State::IMMEDIATE;
				sprintf(message,"Immediate mode on\r\n");
				break;
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
			case '.':
				pduData.pulse_width += 5;
				OCR1A = 2*pduData.pulse_width;
				OCR1B = 2*pduData.pulse_width;
				OCR2A = pduData.pulse_width/16;
				OCR2B = pduData.pulse_width/16;
				sprintf(message,"Pulse width %u\r\n",pduData.pulse_width);
				break;
			case ',':
				pduData.pulse_width -= 5;
				OCR1A = 2*pduData.pulse_width;
				OCR1B = 2*pduData.pulse_width;
				OCR2A = pduData.pulse_width/16;
				OCR2B = pduData.pulse_width/16;
				sprintf(message,"Pulse width %u\r\n",pduData.pulse_width);
				break;
			case 'a':
				volts_reading = Analog::immediate_read(6);
				Analog::adc_scale<MAX_VOLTS>(volts_reading, &amps, &centiamps);
				sprintf(message,"%2u.%02u A  \r\n", amps, centiamps);
				break;
			case 'p':
				if (TCCR1B & PRESCALER1_MASK) {
					TCCR1B &= ~PRESCALER1_MASK;
					TCCR2B &= ~PRESCALER2_MASK;
					sprintf(message,"Timer1 stopped\r\n");
					} else {
					TCCR1B |= PRESCALER1_8;
					TCCR2B |= PRESCALER2_256;
					sprintf(message,"Timer1 started\r\n");
				}
				break;
			case 'v':
				amps_reading = Analog::immediate_read(7);
				Analog::adc_scale<MAX_AMPS>(amps_reading, &volts, &centivolts);
				sprintf(message,"%2u.%02u V  \r\n", volts, centivolts);
				break;
			case 'w':
				volts_reading = Analog::immediate_read(6);
				amps_reading = Analog::immediate_read(7);
				watts_raw = uint32_t(volts_reading) * uint32_t(amps_reading);
				watts = (watts_raw * 39) >> 16;
				centiwatts = ((watts_raw % (watts * 1680)) * 3901) >> 16;
				sprintf(message, "%2lu.%02lu W  \r\n", watts, centiwatts);
				break;
			case 'x':
				pduData.current_state = State::NORMAL;
				sprintf(message,"Normal mode on\r\n");
				break;
			case 'e':
				_raise_error(ErrorType::TEST);
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

void poll_serial_and_process() {
	static bool char_available(false);
	cli(); // Pause interrupts while reading available char
	char c(Serial::get_char(char_available));
	sei(); // Re-enable interrupts
	if (char_available) process(c);
}

int main(void) {
	_error_function = error_function;
	_error_led_function = error_led_function;
	startup_routine(2);
	     
	Serial::set_txden_pin(txden);
	Serial::init(115200);
	
	TCCR1A = (1<<COM1B1) | (1<<COM1A1) | (1<<WGM11);
	TCCR1B = (1<<WGM13) | (1<<WGM12);
	
	TCCR2A = (1<<COM2B1) | (1<<COM2A1) | (1<<WGM21) | (1<<WGM20);
	TCCR2B = 0;//(1<<WGM22);
	
	ICR1 = uint16_t(2*2000);
	OCR1A = 3000;
	OCR1B = 3000;
	
	OCR2A = 94;
	OCR2B = 94;
	
	sei(); // Enable interrupts
	
    while (1) {	
		state_machine[pduData.current_state]();
		poll_serial_and_process();
    }
}

