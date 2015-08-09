/*
 * voltage_monitor.cpp
 *
 * Created: 8/8/2015 7:38:58 PM
 *  Author: Maxim
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRD = 0xff;
	DDRB = 0xff; 
	
	float duty = 0.5f;
	float increment = 0.01f;
	int sign = 1;
	
    while(1)
    {
		for(int i = 0; i < 1023; i++) {
			if(i < duty * 1024) {
				PORTB = 0xff;
			} else
				PORTB = 0x00;
		}
		
		duty += sign * increment;
		
		if (duty > 1) {
			
			duty = 1;
			sign = -1;
		}
		
		if (duty < 0) {
			
			duty = 0;
			sign = 1;
		}
    }
}