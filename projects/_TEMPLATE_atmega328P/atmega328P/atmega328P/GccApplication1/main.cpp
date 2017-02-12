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
	for (int i(1); i <= 6; ++i) enable[i].clear();
}
// Toggles enable[0] corresponding to error led, pass to _error_led_function
void error_led_function() {
	enable[0].toggle();
}

const uint8_t MAX_VOLTS(25);
const uint8_t MAX_AMPS(25);
char message[128];
uint16_t volts_reading, volts, centivolts;
uint16_t amps_reading, amps, centiamps;
uint32_t watts_raw, watts, centiwatts;  

enum State {
	IDLE,
	IMMEDIATE,
	DRUM,
	RECEIVE,
	RESPOND,
	UPDATE_VARIABLES,
	UPDATE_ADC,
	__States_sz__
};

#define NAME "@pdu"

struct IncomingData {
	uint8_t enable_status;
	uint8_t pwm_status;
	uint16_t pwm[6];
};

struct OutgoingData {
	uint16_t voltage_reading;
	uint16_t current_reading;
};

struct PduData {
	const char name[sizeof(NAME)];
	State current_state;
	uint8_t last_enable_status;
	uint16_t current_pwm[6];
	IncomingData incoming;
	OutgoingData outgoing;
};

volatile PduData pduData = {
	NAME,
	State::UPDATE_VARIABLES, 
	0x00,
	{ 0,0,0,0,0,0, },
	{ 0x3F,0,0,0,0,0,0,0 },
	{ 0x55,0x55 }
};

void immediate_function(char c);
void process(char c);
bool char_available(false);

void state_idle(volatile PduData* data) {
	static uint16_t stage(0);
	data->current_state = State::UPDATE_ADC; // Will update ADC by default
	
	cli(); // Pause interrupts while reading available char
	char c(Serial::get_char(char_available));
	sei(); // Re-enable interrupts
	
	if (char_available) {
		if (stage == sizeof(data->name)-1) {
			process(c);
			stage = 0;
		} else {
			if (c == data->name[stage])
				stage++;
			else
				stage = 0;
		}
	}
}

void state_update_adc(volatile PduData* data) {
	static bool check_voltage(true);
	if (check_voltage) {
		data->outgoing.voltage_reading = Analog::immediate_read(6);
	} else {
		data->outgoing.current_reading = Analog::immediate_read(7);
	}
	check_voltage = !check_voltage;
	data->current_state = State::IDLE;
}

void state_update_variables(volatile PduData* data) {
	if (data->incoming.enable_status != data->last_enable_status) {
		for(int i(1); i <= 6; ++i) {
			if (data->incoming.enable_status & (1<<(i-1))) {
				enable[i].set();
				} else {
				enable[i].clear();
			}
		}
		data->last_enable_status = data->incoming.enable_status;
	}
	for(int i(0); i < 6; ++i) {
		if (data->incoming.pwm_status & (1<<i)) {
			data->current_pwm[i] = data->incoming.pwm[i];
		} 
	}
	if (data->current_pwm[0] > 0) {
		TCCR0B |= PRESCALER2_256;
		OCR0A = data->current_pwm[0]/32;
	} else {
		TCCR0B &= ~PRESCALER2_MASK;
	}
	if (data->current_pwm[1] > 0) {
		TCCR0B |= PRESCALER2_256;
		OCR0B = data->current_pwm[1]/32;
	} else {
		TCCR0B &= ~PRESCALER2_MASK;
	}
	if (data->current_pwm[2] > 0) {
		TCCR1B |= PRESCALER1_8;
		OCR1A = data->current_pwm[2]*2;
	} else {
		TCCR1B &= ~PRESCALER1_MASK;
	}
	if (data->current_pwm[3] > 0) {
		TCCR1B |= PRESCALER1_8;
		OCR1B = data->current_pwm[3]*2;
	} else {
		TCCR1B &= ~PRESCALER1_MASK;
	}
	data->current_state = State::IDLE;
}

void state_immediate(volatile PduData* data) {
	cli(); // Pause interrupts while reading available char
	char c(Serial::get_char(char_available));
	sei(); // Re-enable interrupts
		
	if (char_available) immediate_function(c);
}

void state_receive(volatile PduData* data) {
	cli(); // Pause interrupts while reading available char
	char c(Serial::get_char(char_available));
	sei(); // Re-enable interrupts
	
	static uint8_t i(0);
	if (char_available) {
		((char*)(&(data->incoming)))[i] = c;
		i++;
		if (i == sizeof(data->incoming)) {	
			i = 0;
			data->current_state = State::UPDATE_VARIABLES;
		}
	}
}

void state_respond(volatile PduData* data) {
	Serial::transmit((char*)(&(data->outgoing)),sizeof(data->outgoing));
	data->current_state = State::IDLE;
}

void state_drum(volatile PduData* data) {
	const uint8_t vel[] = {
		2,3,2,3,8,3,2,3,2,3,2,3,8,3,2,3,
		2,3,2,3,8,3,2,3,2,3,2,3,8,0,8,0
	};
	for (int i(0); i < 32; ++i) {
		state_immediate(data); // Enters this state to poll incoming data
		for (int j(0); j < 10; ++j) {
			if (vel[i] > j) {
				enable[1].set();
			} else {
				enable[1].clear();
			}
			_delay_ms(10);
		}
		_delay_ms(70);
	}
}

void (*state_machine[__States_sz__])(volatile PduData*) = {
	state_idle, 
	state_immediate, 
	state_drum,
	state_receive,
	state_respond,
	state_update_variables,
	state_update_adc
};

void process(char c) {
	switch(c) {
		case ':':
			pduData.current_state = State::RECEIVE;
			message[0] = 0;
			break;
		case '?':
			pduData.current_state = State::RESPOND;
			message[0] = 0;
			break;
		case '!':
			pduData.current_state = State::IMMEDIATE;
			sprintf(message,
				"\r\nImmediate mode on\r\n"
				"---------------\r\n"
				"Valid commands:\r\n"
				"---------------\r\n"
				"[1-6]: toggle enables\r\n"
			);
			Serial::transmit(message,strlen(message));
			while (!Serial::transmission_complete()) {
				// Wait for first part of message to go out, 128 char buffer size
			}
			sprintf(message,
				"[a]  : print current\r\n"
				"[v]  : print voltage\r\n"
				"[w]  : print power\r\n"
				"[!]  : exit immediate mode\r\n"
				"[s]  : servo test (unimpl.)\r\n"
			);
			break;
		default:
			sprintf(message,"Bad command\r\n");
			break;
	}
	Serial::transmit(message,strlen(message));
}

void immediate_function(char c) {
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
		case 'a':
			volts_reading = Analog::immediate_read(6);
			Analog::adc_scale<MAX_VOLTS>(volts_reading, &amps, &centiamps);
			sprintf(message,"%2u.%02u A  \r\n", amps, centiamps);
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
		case '!':
			pduData.current_state = State::IDLE;
			sprintf(message,"Idle mode on\r\n");
			break;
		default:
			sprintf(message,"Bad command\r\n");
			break;
	}
	Serial::transmit(message,strlen(message));
}

int main(void) {
	_error_function = error_function;
	_error_led_function = error_led_function;
	startup_routine(2);
	
	TCCR0A = (1<<COM0B1) | (1<<COM0A1) | (1<<WGM01) | (1<<WGM00);
	TCCR0B = 0;
	
	TCCR1A = (1<<COM1B1) | (1<<COM1A1) | (1<<WGM11);
	TCCR1B = (1<<WGM13) | (1<<WGM12);
	
	TCCR2A = (1<<COM2B1) | (1<<COM2A1) | (1<<WGM21) | (1<<WGM20);
	TCCR2B = 0;
	
	ICR1 = 40000;
	OCR1A = 3000;
	OCR1B = 3000;
	
	OCR2A = 94;
	OCR2B = 94;
		 
	Serial::set_txden_pin(txden);
	Serial::init(115200);
	Analog::start_conversion();
	
	sei(); // Enable interrupts
	
    while (1) state_machine[pduData.current_state](&pduData);
}

