#ifndef PID_H
#define PID_H

/*
 * PID.h
 *
 * March 31
 *  Author: Max Prokopenko
 *  max@theprogrammingclub.com
 */

using namespace std;
#define INTEGRAL_MARGIN 10

class PID {
	private:
	
		float Kp, Ki, Kd;
		int16_t lastE, E, P, I, D;
		
	public:
	
		PID(void) {};
			
		float compute(uint16_t desired, uint16_t actual) {
			
			lastE = E;
			E = desired - actual;
			
			P = E;
			I = I + E;
			D = E - lastE;
			
			return (float)(Kp * P + Ki * I + Kd * D);
		}
		
		void setGains(float kp, float ki, float kd) {
			
			this->Kp = kp;
			this->Ki = ki;
			this->Kd = kd;
		}
		
		void reset(void) {
			P = I = D = 0;
		}
};

#endif