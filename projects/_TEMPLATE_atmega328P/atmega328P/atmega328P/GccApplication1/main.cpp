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
	uint8_t last_enable_status;
	uint16_t current_pwm[6];
	IncomingData incoming;
	OutgoingData outgoing;
};

volatile PduData pduData = {
	NAME,
	0x00,
	{ 0,0,0,0,0,0, },
	{ 0x3F,0,0,0,0,0,0,0 },
	{ 0x55,0x55 }
};

void immediate_function(char c);
void process(char c);
bool char_available(false);
char last_char('\0');

void inline poll_serial(char& received, bool& available) {
	cli(); // Pause interrupts while reading available char
	received = (Serial::get_char(available));
	sei(); // Re-enable interrupts
}

// All operating states live in this namespace
namespace state {
	void (*current)(volatile PduData*); // Variable holds state for the program

	void update_adc(volatile PduData* data); // Prototype declaration

	void idle(volatile PduData* data) {
		static uint16_t stage(0);
		poll_serial(last_char, char_available);
		current = update_adc; // Will update ADC by default after this if no other states chosen
		if (char_available) {
			if (stage == sizeof(data->name)-1) {
				process(last_char);
				stage = 0;
			} else {
				if (last_char == data->name[stage])
					stage++;
				else
					stage = 0;
			}
		}
	}

	void update_adc(volatile PduData* data) {
		static bool check_voltage(true);
		if (check_voltage) {
			data->outgoing.voltage_reading = Analog::immediate_read(6);
		} else {
			data->outgoing.current_reading = Analog::immediate_read(7);
		}
		check_voltage = !check_voltage;
		current = idle;
	}

	void update_variables(volatile PduData* data) {
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
		current = idle;
	}

	void immediate(volatile PduData* data) {
		poll_serial(last_char, char_available);
		if (char_available) immediate_function(last_char);
	}

	void receive(volatile PduData* data) {
		poll_serial(last_char, char_available);
		static uint8_t i(0);
		if (char_available) {
			((char*)(&(data->incoming)))[i] = last_char;
			i++;
			if (i == sizeof(data->incoming)) {	
				i = 0;
				current = update_variables;
			}
		}
	}

	void respond(volatile PduData* data) {
		Serial::transmit((char*)(&(data->outgoing)),sizeof(data->outgoing));
		current = idle;
	}
} /* end of namespace state */

void process(char c) {
	switch(c) {
		case ':':
			state::current = state::receive;
			message[0] = 0;
			break;
		case '?':
			state::current = state::respond;
			message[0] = 0;
			break;
		case '!':
			state::current = state::immediate;
			sprintf(message,
				"\r\nImmediate mode on\r\n"
				"---------------\r\n"
				"Valid commands:\r\n"
				"---------------\r\n"
				"[1-6]: toggle enables\r\n"
			);
			Serial::transmit(message,strlen(message));
			while (!Serial::transmission_complete()) { /* Wait for first part of message to go out, 128 char buffer size */ }
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
			state::current = state::idle;
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
	
	// Setup peripherals and starting state
	Serial::set_txden_pin(txden);
	Serial::init(115200);
	Analog::start_conversion();
	state::current = state::update_variables;
	
	sei(); // Enable interrupts
	
    while (1) state::current(&pduData); // Executes current state function
}

