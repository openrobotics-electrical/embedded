/*
 * v1_drivetrain_rev2.ino
 *
 * Created: 9/16/2015 2:15:17 PM
 * Author: Maxim
 * Email: max.prokopenko@gmail.com
 * Slack: maxlazarus
 */ 

#define F_CPU 16000000L

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "v1_drivetrain_rev2.h"

volatile char* transmitting;
volatile uint8_t chars_left = 0, chars_sent = 0;

volatile char input_string[256];
volatile uint8_t chars_read = 0, chars_received = 0;
volatile bool received_flag = false;

void onReceived(volatile char* s, uint8_t char_count) 
{
	received_flag = true;
}

void serialEvent()
{
	while (Serial.available())
	{
		volatile char in = (volatile char)Serial.read();
		input_string[chars_read] = in;
		chars_read++;

		if (true) // in == '\n')
		{
			onReceived(input_string, chars_read);
			chars_read = 0;
			chars_received = chars_read;
		}
	}
}

ISR(USART_TX_vect) {
	
	if(chars_left-- > 0) {
		
		UDR0 = transmitting[chars_sent++];
		
		} else {
		
		chars_sent = 0;
	}
}

void serial_init() {
	
	UBRR0H = 0;
	UBRR0L = 16; // BAUD 115200
	UCSR0A = _BV(U2X0);
	UCSR0B = _BV(TXCIE0) | _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

void serial_transmit(char* s, uint8_t char_count) {
	
	// sends between 1 and 255 chars
	
	transmitting = s;
	UDR0 = s[0];
	chars_left = char_count - 1;
	chars_sent++;
}

void setup()
{
	Serial.begin(115200);
	// serial_init();
	// sei();
	
	// setup these pins as outputs
	M1A_DDR = EN1A | IN1A;
	M1B_DDR = EN1B | IN1B;
	M2A_DDR = EN1A | IN1A;
	M2B_DDR = EN2B | IN2B;
	PWM_DDR = PWM1 | PWM2;
	PWM_PORT = PWM1 | PWM2;
	
	set_PWM(0.0f, 0.1f);
	motors_enable(true);
}

volatile uint8_t time = 0;
volatile uint16_t s_count = 0;

void loop()
{
	if(received_flag) {
		
		uint8_t input_value = (uint8_t)input_string[0] - '0';
		set_PWM(0, (input_value * 1.3333 + 5) / 20.0f);
		chars_received = 0;
		received_flag = false;
	}
	
	time = time > time_steps? 0 : time + 1;
	
	PWM_PORT = time < M1_PWM? PWM_PORT | PWM1 : PWM_PORT & ~PWM1;
	M1A_PORT =  M1_direction? M1A_PORT | IN1A : M1A_PORT & ~IN1A;
	M1B_PORT = !M1_direction? M1B_PORT | IN1B : M1B_PORT & ~IN1B;
	
	PWM_PORT = time < M2_PWM? PWM_PORT | PWM2 : PWM_PORT & ~PWM2;
	M2A_PORT =  M2_direction? M2A_PORT | IN2A : M2A_PORT & ~IN2A;
	M2B_PORT = !M2_direction? M2B_PORT | IN2B : M2B_PORT & ~IN2B;
	
	_delay_us(10);
}

void motors_enable(bool on) 
{	
	M1A_PORT = on? EN1A | M1A_PORT : EN1A & ~M1A_PORT;
	M1B_PORT = on? EN1B | M1B_PORT : EN1A & ~M1B_PORT;
	M2A_PORT = on? EN2A | M2A_PORT : EN1A & ~M2A_PORT;
	M2B_PORT = on? EN2B | M2B_PORT : EN1A & ~M2B_PORT;
}

void set_PWM(float M1, float M2) // range -1.0f to 1.0f
{	
	M1_PWM = abs(M1) * time_steps;
	M1_direction = M1 > 0? CLOCKWISE : COUNTERCLOCKWISE;
	M2_PWM = abs(M2) * time_steps;
	M2_direction = M2 > 0? CLOCKWISE : COUNTERCLOCKWISE;
}
