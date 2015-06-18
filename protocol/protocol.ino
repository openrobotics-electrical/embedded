#define DEVICE_NUMBER 2
#define DEVICE_NAME bjkb 

#include <Wire.h>

typedef struct wireData WireData;
struct wireData {
  
  bool hasNewData;
  uint8_t request;
  char name16[16];
};

WireData data = { false, 0x55 };

void setup() {
  
  DDRB = DDRB | 0x20;

  sprintf(data.name16, "ultrasounds      ");
  
  Wire.begin(DEVICE_NUMBER);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}
 
uint8_t request = '!';
boolean newData = false;

void requestEvent() {

  if(request == 0) {
  
    Wire.write(data.name16, 16);
  } 
  else 
  if(request == 1) {
   
    Wire.write(101);
  } 
  else 
  if(request == 2) {
    
    uint8_t outBuffer[2];
    outBuffer[0] = 0x55;
    outBuffer[1] = 0x55;
    Wire.write(outBuffer, 2);
  }
}

void receiveEvent(int args) {
     
    request = Wire.read();
    newData = true;
}

void loop() {
  
  delay(100);
  PORTB = PINB ^ 0x20; 
/*
Bit trig(2, PORTD);
	Bit echo(3, PORTD);
	
	trig.setAsOutput();
	echo.setAsHighZInput();
	
	while(1) {
		
		uint16_t uCount = 0;
		
		trig.clear();
		_delay_us(20);	
		trig.set();
		_delay_us(20);
		trig.clear();
		
		while(echo.isLow()) {};
			
		while(echo.isHigh()) {
			
			_delay_us(1);
			uCount++;
		}
		
		sprintf(strOut, "%u mm\n", (uint16_t)(uCount * 0.25));
		USART_Send_string(strOut);
		_delay_ms(50);
	}
*/
}

