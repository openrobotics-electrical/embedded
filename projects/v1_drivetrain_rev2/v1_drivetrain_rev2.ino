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
	
	M1_output = -0.1f;
	M2_output = 0.55f;
	motors_enable(true);
}

void loop()
{
	M1A_PORT = M1A_PORT ^ EN1A;
	
	static float time = 0; // range 0 to 1
	time = time > 1? 0 : time + 0.01f; // 100 time steps
	
	// set motor 1 PWM
	M1A_PORT = time < M1_output? M1A_PORT & ~IN1A : M1A_PORT | IN1A;
	M1B_PORT = time < -M1_output? M1B_PORT | IN1B : M1B_PORT & ~IN1B;
	
	// set motor 2 PWM
	M2A_PORT = time < M2_output? M2A_PORT | IN2A : M2A_PORT & ~IN2A;
	M2B_PORT = time < -M2_output? M2B_PORT | IN2B : M2B_PORT & ~IN2B;
}

void motors_enable(bool on) 
{	
	M1A_PORT = on? EN1A : 0;
	M1B_PORT = on? EN1B : 0;
	M2A_PORT = on? EN2A : 0;
	M2B_PORT = on? EN2B : 0;
}