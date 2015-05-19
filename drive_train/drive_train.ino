
/*
  Drivers if programming is not going smoothly:
  http://www.ftdichip.com/Drivers/VCP.htm
*/
#include <math.h>  // requires an Atmega168 chip   
#include "Encoder.h"
#include "PID_v1.h" // Include PID library for closed loop control
#include <Wire.h>
#include <avr/interrupt.h>
  
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


///////////////////////////////////////////////////////
  
  //Tethered control: ANALOG PINS
  const int        twistRemotePin = 10, linearRemotePin = 11, button = 12;
  
  double val0 = 200; // Duty Cycle val = 0 gives 0% DUTY, val = 254 gives 100% DUTY
  double rightPotValue = 200; // Duty Cycle val = 0 gives 0% DUTY, val = 254 gives 100% DUTY
  
  const int        INA1 = 22; // These pins control the state of 
  const int        INB1 = 28; // the bridge in normal operation: 
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
  int Kp = 75, Kd = 0, Ki = 0;
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
  boolean mode = TURN_ROBOT;
  int x = 0;

void setup() {

	    
	Serial.begin(9600);
	/*
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
  
	digitalWrite(RST_, 0);
	digitalWrite(EN_, 0);
	digitalWrite(MS1, 1);
	digitalWrite(MS2, 1);
	digitalWrite(MS3, 1);
	digitalWrite(RST_, 1);
	digitalWrite(SLP_, 1);
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	sei();
	
	*/
	Wire.begin(1);
	Wire.onRequest(requestEvent);
	Wire.onReceive(receiveEvent);
}

uint16_t thisMillis, position;
uint16_t buffer[] = {1};

void requestEvent() {

	Serial.println("request to 1 received");
	
	uint8_t outBuffer[3];
	outBuffer[0] = position;
	outBuffer[1] = position / 0x100;
	outBuffer[2] = 0;
	Wire.write(outBuffer, 3);
}

void receiveEvent(int args) {
	
	Serial.println(String(args) + " args received");
	while(Wire.available() > 0)
		Serial.println(Wire.read());
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

void loop() { 
	position = analogRead(0);
	delay(100);
	// (mode == TURN_ROBOT)? turn() : drive();
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
 
  _leftMotorRPMset = getLeftTwistRPM() + getLinearRPM();
  _rightMotorRPMset = getRightTwistRPM() + getLinearRPM();
 
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
 
  int i = 0;
  for(; i < SampleTime; i++) {
    if(readMode()) 
		i == SampleTime;
  }
}

void turn() {
  
	x = analogRead(linearRemotePin);
	// Serial.println(x);
	if (x > 900 && digitalRead(LRSW)) {
		digitalWrite(DIR, LEFT);
		digitalWrite(STEP, 1); // STEP
		delayMicroseconds(100);
	} else if (x < 100 && digitalRead(RRSW)) {
		digitalWrite(DIR, RIGHT);
		digitalWrite(STEP, 1); // STEP
		delayMicroseconds(100);
	}
  
	digitalWrite(STEP, 0);
	delay(1);
	readMode();
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
//  Serial.println(_rightCount);
  _rightMotorRPM = getRPM(_rightCount, SampleTime, CountsPerRotation, MultiplicationFactor);
  
  if (_newRightEncoderPosition != _oldRightEncoderPosition) 
  {
  _oldRightEncoderPosition = _newRightEncoderPosition;
//  Serial.println(_rightMotorRPM);
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
  //Serial.println(_leftMotorRPM);
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

