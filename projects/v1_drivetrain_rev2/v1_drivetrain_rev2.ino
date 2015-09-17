/*
 * v1_drivetrain_rev2.ino
 *
 * Created: 9/16/2015 2:15:17 PM
 * Author: Maxim
 * Email: max.prokopenko@gmail.com
 * Slack: maxlazarus
 */ 

#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>
#include "v1_drivetrain_rev2.h"

void setup()
{
	// setup these pins as outputs
	M1A_DDR = EN1A | IN1A;
	M1B_DDR = EN1B | IN1B;
	M2A_DDR = EN1A | IN1A;
	M2B_DDR = EN2B | IN2B;
	
	set_PWM(0.1f, 0.1f);
	motors_enable(true);
}

void loop()
{
	static uint8_t time = 0;
	time = time > time_steps? 0 : time + 1;
	
	// set motor outputs
	M1A_PORT = time <  M1_output? M1A_PORT | IN1A : M1A_PORT & ~IN1A;
	M1B_PORT = time < -M1_output? M1B_PORT | IN1B : M1B_PORT & ~IN1B;
	M2A_PORT = time <  M2_output? M2A_PORT | IN2A : M2A_PORT & ~IN2A;
	M2B_PORT = time < -M2_output? M2B_PORT | IN2B : M2B_PORT & ~IN2B;
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
	M1_output = M1 * time_steps;
	M2_output = M2 * time_steps;
}