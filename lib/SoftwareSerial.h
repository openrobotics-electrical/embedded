#ifndef _SOFTWARESERIAL_H
#define _SOFTWARESERIAL_H

//------------------CLASSES

class SoftwareSerial {

	private:

//------VARIABLES

		Bit noninverting, inverting;

	public:

//------CONSTRUCTOR

		SoftwareSerial(uint8_t Bplus, uint8_t Aminus, uint8_t port) {
			this->noninverting = Bit(Bplus, port);
			this->inverting = Bit(Aminus, port);
		}	

//------FUNCTIONS
	
		size_t write(uint8_t byte) {
			
			if(byte > 0) {
				
				noninverting.set();
				inverting.clear();
				
			} else {
				
				noninverting.clear();
				inverting.set();
			}
			return 0;
		};
		
		int available();
		
		int read() {
			return 0;
		}
		
		int peek();
		void begin(long speed);
		void end();
		void setRX(uint8_t transmitPin);
		void setTX(uint8_t receivePin);
		void setMode(uint8_t mode) {
			
			if(mode == 1) {
				
				inverting.setAsOutput();
				noninverting.setAsOutput();
				
			} else {
				
				inverting.setAsHighZInput();
				noninverting.setAsHighZInput();
			}
		}
};

#endif