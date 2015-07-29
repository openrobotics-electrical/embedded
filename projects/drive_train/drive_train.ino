
/*
  Drivers if programming is not going smoothly:
  http://www.ftdichip.com/Drivers/VCP.htm
*/
#include <math.h>  // requires an Atmega168 chip   
#include "Encoder.h"
#include "PID_v1.h" // Include PID library for closed loop control
#include <Wire.h>
#include "I2C.h"

/*
  Stolen from Arduino as ISP
*/
#include "pins_arduino.h"
#define RESET     SS

#define LED_HB    9
#define LED_ERR   8
#define LED_PMODE 7
#define PROG_FLICKER true

#define HWVER 2
#define SWMAJ 1
#define SWMIN 18

// STK Definitions
#define STK_OK      0x10
#define STK_FAILED  0x11
#define STK_UNKNOWN 0x12
#define STK_INSYNC  0x14
#define STK_NOSYNC  0x15
#define CRC_EOP     0x20 //ok it is a space...
  
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


///////////////////////////////////////////////////////
  
  //Tethered control: ANALOG PINS
  const int        twistRemotePin = 10, linearRemotePin = 11, button = 12;
  
  double val0 = 200; // Duty Cycle val = 0 gives 0% DUTY, val = 254 gives 100% DUTY
  double rightPotValue = 200; // Duty Cycle val = 0 gives 0% DUTY, val = 254 gives 100% DUTY
  
  const int        INA1 = 28; // These pins control the state of 
  const int        INB1 = 22; // the bridge in normal operation: 
  const int        INA2 = 30;
  const int        INB2 = 36;
  const int        ENA1 = 24; //LOW Disables Half Bridge A HIGH Enables half bridge A
  const int        ENB1 = 26; //LOW Disables Half Bridge B HIGH Enables half bridge B
  const int        ENA2 = 32;
  const int        ENB2 = 34;
  const int        PWMpin1 = 7;
  const int        PWMpin2 = 6;
  #define LAD A8
  #define RAD A9
  
  // TURNTABLE
  
  #define EN_ 39
  #define MS1 52
  #define MS2 52
  #define MS3 52
  #define RST_ 47
  #define SLP_ 49
  #define STEP 51
  #define DIR 53
  
  #define LRSW 9
  #define RRSW 10
  #define HES 8
  
  //Encoder count variables:   
  double           _leftMotorRPM = 0;
  double           _rightMotorRPM = 0;
  long             _newLeftEncoderPosition = 0;
  long             _newRightEncoderPosition = 0;
  long             _oldLeftEncoderPosition = 0;
  long             _oldRightEncoderPosition  = 0;
  float            _leftCount = 0;
  float            _rightCount = 0;
  
  const float      CountsPerRotation = 4680;
  const float      MultiplicationFactor = 6; //counts per encoder impulse
  
  //delay
  uint16_t lastMillis;
  
  //PID Variables:
  double _rightMotorRPMset = 0, _rightInput = 0, _rightOutput = 0;
  double _leftMotorRPMset = 0, _leftInput = 0, _leftOutput = 0;
  const float SampleTime = 50;
  int Kp = 50, Kd = 0, Ki = 0;
  int KpIN = 0, KdIN = 0, KiIN = 0;
  int KpPin = 5, KdPin = 6, KiPin = 7;
  double MaxRPM = 1.45;
  
  //Joystick Control Variables:
  double linearXHigh = 1023; 
  double linearXLow = 135;
  double linearXCentre = 560;
  double linearAnalogPin = 3;
  double twistXHigh = 1023;
  double twistXLow = 125;
  double twistXCentre = 480;
  
  //Initialize encoder inputs
  Encoder LeftEncoder(2, 3);
  Encoder RightEncoder(18,19);
  
  // Initialize PID
  PID rightPID(&_rightInput, &_rightOutput, &_rightMotorRPMset, Kp, Ki, Kd, DIRECT);
  PID leftPID(&_leftInput, &_leftOutput, &_leftMotorRPMset, Kp, Ki, Kd, DIRECT);

  #define TURN_ROBOT true
  #define DRIVE_ROBOT false
  #define LEFT 1
  #define RIGHT 0
  boolean mode = TURN_ROBOT, newData = false;
  int leftTwistRPM, rightTwistRPM, linearRPM;
  int x = 0;

void setup() {
  
	Serial.begin(57600);

	pinMode(INA1, OUTPUT);
	pinMode(INB1, OUTPUT);  
	pinMode(INA2, OUTPUT);
	pinMode(INB2, OUTPUT);
	pinMode(ENA1, OUTPUT);
	pinMode(ENB1, OUTPUT);
	pinMode(ENA2, OUTPUT);
	pinMode(ENB2, OUTPUT);
  
	//--------------TURNTABLE CONTROL 
  
	pinMode(EN_, OUTPUT); 
	pinMode(MS1, OUTPUT);
	pinMode(MS2, OUTPUT);
	pinMode(MS3, OUTPUT);
	pinMode(RST_, OUTPUT); 
	pinMode(SLP_, OUTPUT);
	pinMode(STEP, OUTPUT);
	pinMode(DIR, OUTPUT);
  
	//-------------------------------
  
	pinMode(RRSW, INPUT_PULLUP);
	pinMode(LRSW, INPUT_PULLUP);
	pinMode(HES, INPUT_PULLUP);
  
	pinMode(button, INPUT_PULLUP);
  
	inputString.reserve(200);
    
	// Initialize Velocity Setpoint
	_rightMotorRPMset = 1;  
  
	//turn the PID on
	rightPID.SetMode(AUTOMATIC);
	rightPID.SetSampleTime(SampleTime);
  
	leftPID.SetMode(AUTOMATIC);
	leftPID.SetSampleTime(SampleTime);
    
	enableMotors();
        leftTwistRPM = rightTwistRPM = linearRPM = 0;
  
	digitalWrite(RST_, 0);
	digitalWrite(EN_, 0);
	digitalWrite(MS1, 1);
	digitalWrite(MS2, 1);
	digitalWrite(MS3, 1);
	digitalWrite(RST_, 1);
	digitalWrite(SLP_, 0);

	Wire.begin(1);
	Wire.onRequest(requestEvent);
	Wire.onReceive(receiveEvent);
}

uint16_t thisMillis, position;
uint16_t buffer[] = {1};
uint8_t request = 0;

char name16[] = "drive_train     "; 

void requestEvent() {

  if(request == 0) {
  
    Wire.write(name16, 16);
  } 
  else 
  if(request == 1) {
   
    Wire.write(99);
  } 
  else 
  if(request == 2) {
    
    uint8_t outBuffer[2];
    outBuffer[0] = position;
    outBuffer[1] = position / 0x100;
    Wire.write(outBuffer, 2);
  }
}

void receiveEvent(int args) {
   
  // while(Wire.available() > 0)
    request = Wire.read();
    newData = true;
}

void enableMotors() {
  
	digitalWrite(ENA1, 1);
	digitalWrite(ENB1, 1);
	digitalWrite(ENA2, 1);
	digitalWrite(ENB2, 1);
}

/*
ISR(USART0_RX_vect) {
	
	digitalWrite(13, HIGH);   // set the LED on
	delay(1000);              // wait for a second
	digitalWrite(13, LOW);
}
*/

typedef struct controlMessage {
  uint8_t something;
};

void loop() { 
  
  if(digitalRead(48) == 0) {
  
    if(newData) {
      
      if((char)request == 'l') 
        turn(LEFT, 200);
      else if ((char)request == 'r') 
        turn(RIGHT, 200);
      else if ((char)request == 'f') {
        linearRPM = 5;
      } else if ((char)request == 'b') {
        linearRPM = -5;
      }
        
      newData = false;
    }
    
    if(linearRPM != 0) {
      
      for(int i = 0; i < 10; i++) {
        
        delay(50);
        drive();
      }
      
      linearRPM = 0;
    }
    
    drive();
    
  } else {
    
    setup_icsp();
    
    while(digitalRead(48) == 1)
      icsp();
  }
}

void testHall() {
	
	int hallHigh, hallLow;
	
	while(1) {
		hallHigh = 0;
		hallLow = 0;
		while(digitalRead(HES) == 1);
		while (digitalRead(HES) == 0);
		while(digitalRead(HES) == 1) {
			hallHigh++;
		}
		while(digitalRead(HES) == 0) {
			hallLow++;
		}
		Serial.print("H: ");
		Serial.print(hallHigh);
		Serial.print(" L: ");
		Serial.println(hallLow);
	}
}

void drive() {
  
  rightPID.SetTunings(Kp, Ki, Kd);
  leftPID.SetTunings(Kp, Ki, Kd);
 
  _leftMotorRPMset = leftTwistRPM + linearRPM;
  _rightMotorRPMset = rightTwistRPM + linearRPM; 
 
  setMotorDirection(_leftMotorRPMset, INA1, INB1);  //Sets Pin outs for Pololu 705 
  setMotorDirection(_rightMotorRPMset, INA2, INB2);
 
  _leftMotorRPMset = abs(_leftMotorRPMset);
  _rightMotorRPMset = abs(_rightMotorRPMset);
 
  computeLeftRPM();   
  computeRightRPM();                                     //RIGHT motorSpeed compute
 
  _leftInput = _leftMotorRPM;
  _rightInput = _rightMotorRPM;

  leftPID.Compute();                              //Compute new PID values given input = _rightMotorRPM
  rightPID.Compute();
 
  analogWrite(PWMpin1, _leftOutput); 
  analogWrite(PWMpin2, _rightOutput);
}

void turn(uint8_t dir, uint8_t steps) {
  
  digitalWrite(SLP_, 1);
 
  for(; steps > 0; steps--) {
    
    digitalWrite(DIR, dir);
    digitalWrite(STEP, 1); // STEP
    delayMicroseconds(100);
    digitalWrite(STEP, 0);
    delay(10);
  }
  
  digitalWrite(SLP_, 0);
}

boolean readMode() {
	
	int modeSwitch = analogRead(button);
	delayMicroseconds(100);
	
	if(modeSwitch <= 10) {
  
		mode = !mode;
		while(modeSwitch <= 10) {
			modeSwitch = analogRead(button);
			delayMicroseconds(150);
		}
		return true;
	}

	return false;
}

void SetPIDValues()
{
       //read PID input values from potentiometers
  KpIN = analogRead(KpPin);
  KdIN = analogRead(KdPin);
  KiIN = analogRead(KiPin);
    
  if(KpIN/2 != Kp){
  Kp = KpIN;
//  Serial.print("Kp: ");
//  Serial.println(Kp);
  rightPID.SetTunings(Kp, Ki, Kd);
  leftPID.SetTunings(Kp, Ki, Kd);
  }
  if(KdIN/2 != Kd){
  Kd = KdIN/2;
 // Serial.println("Kd:");
 // Serial.println(Kd);
  rightPID.SetTunings(Kp, Ki, Kd);
  leftPID.SetTunings(Kp, Ki, Kd);
  }
  if(KiIN/2 != Ki){
  Ki = KiIN;
//  Serial.println("Ki:");
//  Serial.println(Ki);
  rightPID.SetTunings(Kp, Ki, Kd);
  leftPID.SetTunings(Kp, Ki, Kd);
  }
}


//FUNCTION: getCount: This function is used to determine the count of the encoders after rotation of one loop
double getCount( long _oldEncoderPosition, long _newEncoderPosition)
{
  float _Count = 0;
  _Count = _newEncoderPosition - _oldEncoderPosition; //leftCount is the # of (encoder pulses)*(N=6) per "SampleTime"
  
  return _Count;
}

//FUNCTION: getRPM returns ABSOLUTE Value of the shaft velocity

float getRPM(double _Count,  double SampleTime, int CountsPerRotation, int MultiplicationFactor)
{
  double _MotorRPM = 0;
  _MotorRPM = _Count/((SampleTime/1000)*CountsPerRotation*MultiplicationFactor); //Calculates actual RPM of motor shaft
  
  if(_MotorRPM < 0){
    _MotorRPM = -_MotorRPM;
  }
  
  return _MotorRPM;
}

//FUNCTION: ComputeRightRPM

void computeRightRPM()
{
  _newRightEncoderPosition = RightEncoder.read();
  _rightCount = getCount( _oldRightEncoderPosition, _newRightEncoderPosition);
  _rightMotorRPM = getRPM(_rightCount, SampleTime, CountsPerRotation, MultiplicationFactor);
  
  if (_newRightEncoderPosition != _oldRightEncoderPosition) 
  {
  _oldRightEncoderPosition = _newRightEncoderPosition;
  }
}

//FUNCTION: ComputeLeftRPM

void computeLeftRPM()
{
    _newLeftEncoderPosition = LeftEncoder.read();  
    _leftCount = getCount( _oldLeftEncoderPosition, _newLeftEncoderPosition);
 
     if (_newLeftEncoderPosition != _oldLeftEncoderPosition) 
     {
     _leftMotorRPM = getRPM(_leftCount, SampleTime, CountsPerRotation, MultiplicationFactor);
     _oldLeftEncoderPosition = _newLeftEncoderPosition;
  }
}

// FUNCTION: CaclRPMSet

double CalcRPMSet( double XHigh, double XLow, double XCentre, double AnalogInput)
{
  double SlopeHigh = 0, SlopeLow = 0;
  double FHigh = 0, FLow = 0;
  double MinOut = 0.3, MaxOut = (0.9*MaxRPM), output = 0; 
  double X = 0;
  double power = 0;
  X = analogRead(AnalogInput);

  if( X > XCentre){
    SlopeHigh = (MaxRPM/(XHigh-XCentre));
    FHigh = SlopeHigh*(X-XCentre);
  //  Serial.println(X);
    if((FHigh >= MinOut)&&(FHigh <= MaxOut)){
      output = -FHigh;
    }
    else if(FHigh < MinOut){
      output = 0;
    }
    else if(FHigh > MaxOut){
      output = -MaxOut;
    }
  }
  else if( X <= XCentre){
    SlopeLow = (MaxRPM/(XLow-XCentre));
    FLow = SlopeLow*(X-XCentre);
  //  Serial.println(X);
    if((FLow >= MinOut)&&(FLow <= MaxOut)){
      output = FLow;
    }
    else if(FLow < MinOut){
      output = 0;
    }
    else if(FLow > MaxOut){
      output = MaxOut;
    }
  }
  
  return output;
}

//FUNCTION: setMotorDirection

int setMotorDirection(double Velocity, int INA, int INB)
{
  if(Velocity > 0){
    digitalWrite(INA, HIGH);
    digitalWrite(INB, LOW);
  }
  else if (Velocity < 0){
    digitalWrite(INA, LOW);
    digitalWrite(INB, HIGH);
  }
  else{
    digitalWrite(INA, LOW);
    digitalWrite(INB, LOW);
    Velocity = 0;
  }
  return 1;
}

//FUNCTION: getLeftTwistRPM

double getLeftTwistRPM()
{
  double twistRPM = 0;
  double leftTwistRPM = 0;
  double rightTwistRPM = 0;
  twistRPM = CalcRPMSet( twistXHigh, twistXLow , twistXCentre, twistRemotePin);
  leftTwistRPM = -twistRPM;
  
  return leftTwistRPM;
}

//FUNCTION getRightTwistRPM

double getRightTwistRPM()
{
  double twistRPM = 0;
  double leftTwistRPM = 0;
  double rightTwistRPM = 0;
  twistRPM = CalcRPMSet( twistXHigh, twistXLow , twistXCentre, twistRemotePin);
  rightTwistRPM = twistRPM;
  
  return rightTwistRPM;
}

//FUNCTION: getLinearRPM

double getLinearRPM()
{
  double linearRPM = 0;
  linearRPM = CalcRPMSet( linearXHigh, linearXLow , linearXCentre, linearRemotePin);
  
  return linearRPM;
}

  
  //function for printing strings...dunno how it works.  
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      
    if (stringComplete) {
      onReceived(inputString);

    // clear the string:
    inputString = "";
      stringComplete = false;
      }  
    } 
  }
}

void onReceived(String s) {
    if (s == "kp\n") {
      Serial.println(Kp);
    }
    else if (s == "kd\n") {
      Serial.println(Kd);
    }
    else if (s == "ki\n") {
      Serial.println(Ki);
    }
    else if (s == "RPM\n") {
      Serial.println(_rightMotorRPM);
    }
    else if (s == "set\n") {
      Serial.println(_rightMotorRPMset);
    }
        else if (s == "output\n") {
      Serial.println(_rightOutput);
    }
}

// ArduinoISP version 04m3
// Copyright (c) 2008-2011 Randall Bohn
// If you require a license, see
//     http://www.opensource.org/licenses/bsd-license.php
//
// This sketch turns the Arduino into a AVRISP
// using the following arduino pins:
//
// pin name:    not-mega:         mega(1280 and 2560)
// slave reset: 10:               53
// MOSI:        11:               51
// MISO:        12:               50
// SCK:         13:               52
//
// Put an LED (with resistor) on the following pins:
// 9: Heartbeat   - shows the programmer is running
// 8: Error       - Lights up if something goes wrong (use red if that makes sense)
// 7: Programming - In communication with the slave
//
// 23 July 2011 Randall Bohn
// -Address Arduino issue 509 :: Portability of ArduinoISP
// http://code.google.com/p/arduino/issues/detail?id=509
//
// October 2010 by Randall Bohn
// - Write to EEPROM > 256 bytes
// - Better use of LEDs:
// -- Flash LED_PMODE on each flash commit
// -- Flash LED_PMODE while writing EEPROM (both give visual feedback of writing progress)
// - Light LED_ERR whenever we hit a STK_NOSYNC. Turn it off when back in sync.
// - Use pins_arduino.h (should also work on Arduino Mega)
//
// October 2009 by David A. Mellis
// - Added support for the read signature command
//
// February 2009 by Randall Bohn
// - Added support for writing to EEPROM (what took so long?)
// Windows users should consider WinAVR's avrdude instead of the
// avrdude included with Arduino software.
//
// January 2008 by Randall Bohn
// - Thanks to Amplificar for helping me with the STK500 protocol
// - The AVRISP/STK500 (mk I) protocol is used in the arduino bootloader
// - The SPI functions herein were developed for the AVR910_ARD programmer
// - More information at http://code.google.com/p/mega-isp

void pulse(int pin, int times);

void setup_icsp() {
  Serial.begin(19200);
  pinMode(LED_PMODE, OUTPUT);
  pulse(LED_PMODE, 2);
  pinMode(LED_ERR, OUTPUT);
  pulse(LED_ERR, 2);
  pinMode(LED_HB, OUTPUT);
  pulse(LED_HB, 2);
}

int error = 0;
int pmode = 0;
// address for reading and writing, set by 'U' command
int here;
uint8_t buff[256]; // global block storage

#define beget16(addr) (*addr * 256 + *(addr+1) )
typedef struct param {
  uint8_t devicecode;
  uint8_t revision;
  uint8_t progtype;
  uint8_t parmode;
  uint8_t polling;
  uint8_t selftimed;
  uint8_t lockbytes;
  uint8_t fusebytes;
  int flashpoll;
  int eeprompoll;
  int pagesize;
  int eepromsize;
  int flashsize;
}
parameter;

parameter param;

// this provides a heartbeat on pin 9, so you can tell the software is running.
uint8_t hbval = 128;
int8_t hbdelta = 8;
void heartbeat() {
  if (hbval > 192) hbdelta = -hbdelta;
  if (hbval < 32) hbdelta = -hbdelta;
  hbval += hbdelta;
  analogWrite(LED_HB, hbval);
  delay(20);
}


void icsp(void) {
  // is pmode active?
  if (pmode) digitalWrite(LED_PMODE, HIGH);
  else digitalWrite(LED_PMODE, LOW);
  // is there an error?
  if (error) digitalWrite(LED_ERR, HIGH);
  else digitalWrite(LED_ERR, LOW);

  // light the heartbeat LED
  heartbeat();
  if (Serial.available()) {
    avrisp();
  }
}

uint8_t getch() {
  while (!Serial.available());
  return Serial.read();
}
void fill(int n) {
  for (int x = 0; x < n; x++) {
    buff[x] = getch();
  }
}

#define PTIME 30
void pulse(int pin, int times) {
  do {
    digitalWrite(pin, HIGH);
    delay(PTIME);
    digitalWrite(pin, LOW);
    delay(PTIME);
  }
  while (times--);
}

void prog_lamp(int state) {
  if (PROG_FLICKER)
    digitalWrite(LED_PMODE, state);
}

void spi_init() {
  uint8_t x;
  SPCR = 0x53;
  x = SPSR;
  x = SPDR;
}

void spi_wait() {
  do {
  }
  while (!(SPSR & (1 << SPIF)));
}

uint8_t spi_send(uint8_t b) {
  uint8_t reply;
  SPDR = b;
  spi_wait();
  reply = SPDR;
  return reply;
}

uint8_t spi_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  uint8_t n;
  spi_send(a);
  n = spi_send(b);
  //if (n != a) error = -1;
  n = spi_send(c);
  return spi_send(d);
}

void empty_reply() {
  if (CRC_EOP == getch()) {
    Serial.print((char)STK_INSYNC);
    Serial.print((char)STK_OK);
  }
  else {
    error++;
    Serial.print((char)STK_NOSYNC);
  }
}

void breply(uint8_t b) {
  if (CRC_EOP == getch()) {
    Serial.print((char)STK_INSYNC);
    Serial.print((char)b);
    Serial.print((char)STK_OK);
  }
  else {
    error++;
    Serial.print((char)STK_NOSYNC);
  }
}

void get_version(uint8_t c) {
  switch (c) {
    case 0x80:
      breply(HWVER);
      break;
    case 0x81:
      breply(SWMAJ);
      break;
    case 0x82:
      breply(SWMIN);
      break;
    case 0x93:
      breply('S'); // serial programmer
      break;
    default:
      breply(0);
  }
}

void set_parameters() {
  // call this after reading paramter packet into buff[]
  param.devicecode = buff[0];
  param.revision   = buff[1];
  param.progtype   = buff[2];
  param.parmode    = buff[3];
  param.polling    = buff[4];
  param.selftimed  = buff[5];
  param.lockbytes  = buff[6];
  param.fusebytes  = buff[7];
  param.flashpoll  = buff[8];
  // ignore buff[9] (= buff[8])
  // following are 16 bits (big endian)
  param.eeprompoll = beget16(&buff[10]);
  param.pagesize   = beget16(&buff[12]);
  param.eepromsize = beget16(&buff[14]);

  // 32 bits flashsize (big endian)
  param.flashsize = buff[16] * 0x01000000
                    + buff[17] * 0x00010000
                    + buff[18] * 0x00000100
                    + buff[19];

}

void start_pmode() {
  spi_init();
  // following delays may not work on all targets...
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, HIGH);
  pinMode(SCK, OUTPUT);
  digitalWrite(SCK, LOW);
  delay(50);
  digitalWrite(RESET, LOW);
  delay(50);
  pinMode(MISO, INPUT);
  pinMode(MOSI, OUTPUT);
  spi_transaction(0xAC, 0x53, 0x00, 0x00);
  pmode = 1;
}

void end_pmode() {
  pinMode(MISO, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(SCK, INPUT);
  pinMode(RESET, INPUT);
  pmode = 0;
}

void universal() {
  int w;
  uint8_t ch;

  fill(4);
  ch = spi_transaction(buff[0], buff[1], buff[2], buff[3]);
  breply(ch);
}

void flash(uint8_t hilo, int addr, uint8_t data) {
  spi_transaction(0x40 + 8 * hilo,
                  addr >> 8 & 0xFF,
                  addr & 0xFF,
                  data);
}
void commit(int addr) {
  if (PROG_FLICKER) prog_lamp(LOW);
  spi_transaction(0x4C, (addr >> 8) & 0xFF, addr & 0xFF, 0);
  if (PROG_FLICKER) {
    delay(PTIME);
    prog_lamp(HIGH);
  }
}

//#define _current_page(x) (here & 0xFFFFE0)
int current_page(int addr) {
  if (param.pagesize == 32)  return here & 0xFFFFFFF0;
  if (param.pagesize == 64)  return here & 0xFFFFFFE0;
  if (param.pagesize == 128) return here & 0xFFFFFFC0;
  if (param.pagesize == 256) return here & 0xFFFFFF80;
  return here;
}


void write_flash(int length) {
  fill(length);
  if (CRC_EOP == getch()) {
    Serial.print((char) STK_INSYNC);
    Serial.print((char) write_flash_pages(length));
  }
  else {
    error++;
    Serial.print((char) STK_NOSYNC);
  }
}

uint8_t write_flash_pages(int length) {
  int x = 0;
  int page = current_page(here);
  while (x < length) {
    if (page != current_page(here)) {
      commit(page);
      page = current_page(here);
    }
    flash(LOW, here, buff[x++]);
    flash(HIGH, here, buff[x++]);
    here++;
  }

  commit(page);

  return STK_OK;
}

#define EECHUNK (32)
uint8_t write_eeprom(int length) {
  // here is a word address, get the byte address
  int start = here * 2;
  int remaining = length;
  if (length > param.eepromsize) {
    error++;
    return STK_FAILED;
  }
  while (remaining > EECHUNK) {
    write_eeprom_chunk(start, EECHUNK);
    start += EECHUNK;
    remaining -= EECHUNK;
  }
  write_eeprom_chunk(start, remaining);
  return STK_OK;
}
// write (length) bytes, (start) is a byte address
uint8_t write_eeprom_chunk(int start, int length) {
  // this writes byte-by-byte,
  // page writing may be faster (4 bytes at a time)
  fill(length);
  prog_lamp(LOW);
  for (int x = 0; x < length; x++) {
    int addr = start + x;
    spi_transaction(0xC0, (addr >> 8) & 0xFF, addr & 0xFF, buff[x]);
    delay(45);
  }
  prog_lamp(HIGH);
  return STK_OK;
}

void program_page() {
  char result = (char) STK_FAILED;
  int length = 256 * getch();
  length += getch();
  char memtype = getch();
  // flash memory @here, (length) bytes
  if (memtype == 'F') {
    write_flash(length);
    return;
  }
  if (memtype == 'E') {
    result = (char)write_eeprom(length);
    if (CRC_EOP == getch()) {
      Serial.print((char) STK_INSYNC);
      Serial.print(result);
    }
    else {
      error++;
      Serial.print((char) STK_NOSYNC);
    }
    return;
  }
  Serial.print((char)STK_FAILED);
  return;
}

uint8_t flash_read(uint8_t hilo, int addr) {
  return spi_transaction(0x20 + hilo * 8,
                         (addr >> 8) & 0xFF,
                         addr & 0xFF,
                         0);
}

char flash_read_page(int length) {
  for (int x = 0; x < length; x += 2) {
    uint8_t low = flash_read(LOW, here);
    Serial.print((char) low);
    uint8_t high = flash_read(HIGH, here);
    Serial.print((char) high);
    here++;
  }
  return STK_OK;
}

char eeprom_read_page(int length) {
  // here again we have a word address
  int start = here * 2;
  for (int x = 0; x < length; x++) {
    int addr = start + x;
    uint8_t ee = spi_transaction(0xA0, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF);
    Serial.print((char) ee);
  }
  return STK_OK;
}

void read_page() {
  char result = (char)STK_FAILED;
  int length = 256 * getch();
  length += getch();
  char memtype = getch();
  if (CRC_EOP != getch()) {
    error++;
    Serial.print((char) STK_NOSYNC);
    return;
  }
  Serial.print((char) STK_INSYNC);
  if (memtype == 'F') result = flash_read_page(length);
  if (memtype == 'E') result = eeprom_read_page(length);
  Serial.print(result);
  return;
}

void read_signature() {
  if (CRC_EOP != getch()) {
    error++;
    Serial.print((char) STK_NOSYNC);
    return;
  }
  Serial.print((char) STK_INSYNC);
  uint8_t high = spi_transaction(0x30, 0x00, 0x00, 0x00);
  Serial.print((char) high);
  uint8_t middle = spi_transaction(0x30, 0x00, 0x01, 0x00);
  Serial.print((char) middle);
  uint8_t low = spi_transaction(0x30, 0x00, 0x02, 0x00);
  Serial.print((char) low);
  Serial.print((char) STK_OK);
}
//////////////////////////////////////////
//////////////////////////////////////////


////////////////////////////////////
////////////////////////////////////
int avrisp() {
  uint8_t data, low, high;
  uint8_t ch = getch();
  switch (ch) {
    case '0': // signon
      error = 0;
      empty_reply();
      break;
    case '1':
      if (getch() == CRC_EOP) {
        Serial.print((char) STK_INSYNC);
        Serial.print("AVR ISP");
        Serial.print((char) STK_OK);
      }
      break;
    case 'A':
      get_version(getch());
      break;
    case 'B':
      fill(20);
      set_parameters();
      empty_reply();
      break;
    case 'E': // extended parameters - ignore for now
      fill(5);
      empty_reply();
      break;

    case 'P':
      start_pmode();
      empty_reply();
      break;
    case 'U': // set address (word)
      here = getch();
      here += 256 * getch();
      empty_reply();
      break;

    case 0x60: //STK_PROG_FLASH
      low = getch();
      high = getch();
      empty_reply();
      break;
    case 0x61: //STK_PROG_DATA
      data = getch();
      empty_reply();
      break;

    case 0x64: //STK_PROG_PAGE
      program_page();
      break;

    case 0x74: //STK_READ_PAGE 't'
      read_page();
      break;

    case 'V': //0x56
      universal();
      break;
    case 'Q': //0x51
      error = 0;
      end_pmode();
      empty_reply();
      break;

    case 0x75: //STK_READ_SIGN 'u'
      read_signature();
      break;

      // expecting a command, not CRC_EOP
      // this is how we can get back in sync
    case CRC_EOP:
      error++;
      Serial.print((char) STK_NOSYNC);
      break;

      // anything else we will return STK_UNKNOWN
    default:
      error++;
      if (CRC_EOP == getch())
        Serial.print((char)STK_UNKNOWN);
      else
        Serial.print((char)STK_NOSYNC);
  }
}
