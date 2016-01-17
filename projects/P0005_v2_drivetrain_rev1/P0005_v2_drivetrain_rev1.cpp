/*
 * P0005_v2_drivetrain_rev1.cpp
 *
 * Created: 10/18/2015 7:19:53 PM
 *  Author: Maxim
 */ 

#define F_CPU 14745600
// #define F_CPU 16000000

#define PRESCALER_1		0b001
#define PRESCALER_8		0b010
#define PRESCALER_64	0b011
#define PRESCALER_256	0b100
#define PRESCALER_1024	0b101

#define COUNTS_PER_REVOLUTION 663
#define TIMER_COUNT 93 // F_CPU / (2 * 256 * COUNTS_PER_REVOLUTION) - 1
#define MOTOR_PIN _BV(PORTD3)

<<<<<<< HEAD
=======
#include <modular8.h>
#include <analog.h>
#include <s3p.h>

>>>>>>> b5a74c1720be9c61e7d5e9ed57523cb186baa737
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

#include <modular8.h>
#include <Analog.h>
#include <S3P.h>
#include "dataStructures.h"

volatile struct data_in
{
	uint8_t motor1_pwm;
	uint8_t motor2_pwm;
} Data_in;

volatile struct data_out
{
	uint8_t motor1_speed;
	uint8_t motor2_speed;
} Data_out;

volatile int8_t encoder_count = 0;
volatile int8_t measured_vel = 0, desired_vel = 0;
volatile uint8_t last_state = 0;
volatile bool counted = false;

const int8_t encoder_matrix[4][4] = {
	
	// TODO implement error flag for invalid states?
	{ 0,  1, -1,  0 },
	{-1,  0,  0,  1 },
	{ 1,  0,  0, -1 },
	{ 0, -1,  1,  0 }
};

uint8_t PWM_count;
int8_t PWM_signal;

ISR(TIMER0_COMPA_vect) 
{	
	// 128-step resolution PWM 
	
	if((PWM_count = (PWM_count + 1) % 128) == 0) {
		
		measured_vel = encoder_count;
		encoder_count = 0;	
		counted = true;
	}
	
	PORTC = PWM_signal > PWM_count? 
		PINC | MOTOR_PIN : PINC & ~MOTOR_PIN;
}

ISR(PCINT0_vect) 
{
	PORTD = PIND | _BV(PD5);
	encoder_count += encoder_matrix[PINB & 0b11][last_state];
	last_state = PINB & 0b11;
	PORTD = PIND & ~_BV(PD5);
}

int main(void) 
{
	DDRB = 0; // PORTB as inputs
	DDRC = 0xff;
	DDRD = 0xff; // PORTD as outputs
	
	PCICR = _BV(PCIE0); // allow PCINT0-7 interrupts
	PCMSK0 = _BV(PCINT1) | _BV(PCINT0); // trigger interrupts on changes to PB0 and PB1 

	TCCR0A = _BV(WGM01); // CTC mode
	TCCR0B = PRESCALER_1024;
	OCR0A = TIMER_COUNT; // calculated above to give 4/663 of a second
	TIMSK0 = _BV(OCIE1A); // enable timer interrupt
	
<<<<<<< HEAD
	// Sets the buffers to the structs in local "dataStructures.h" and initializes UART
	S3P::init(DATA_STRUCTURE_REF);
	S3P::setDelimiter("@V2DT");
	
	sei(); // set interrupts
	
	DDRC = 0x3f;
	Analog::selectChannel(5);
	Analog::startConversion();
	
	#define DELAY 100
	
    while(1) 
	{
		PORTC = 0x00;
		_delay_ms(DELAY);
		
		sprintf((char*)dataOut.output, "V on: %d\n", Analog::getValue());
		
		_delay_ms(DELAY);
		
		PORTC = 0x00;
		_delay_ms(DELAY);
		
		sprintf((char*)dataOut.output, "V off: %d\n", Analog::getValue());
		
		// S3P::transmit();
		
		_delay_ms(DELAY);
		/*
		if(counted) 
		{	
			PWM_signal += (desired_vel - measured_vel) / 3;
			if(PWM_signal < 0) PWM_signal = 0;
			
			out[0] = measured_vel < 0? '-' : '+';
			out[1] = (abs(measured_vel) / 10) + '0';
			out[2] = (abs(measured_vel) % 10) + '0';
=======
	char name[] = "@MAB";
	
	s3p_setbuffers(REF(Data_in), REF(Data_out));
	s3p_set_delimiter(name, 4);
	s3p_init();
	
	PWM_signal = 3;
	
	sei(); // set interrupts
>>>>>>> b5a74c1720be9c61e7d5e9ed57523cb186baa737
	
    while(1) 
	{	
		if(counted) 
		{			
			// PWM_signal = Data_in.motor1_pwm;
			char test[] = "YARGGH";
			s3p_transmit(test, sizeof(test));
			
			counted = false;
			PORTC = PORTC ^ 0x30;
		}
		*/
    }
}