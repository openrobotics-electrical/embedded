#define TRIG_A 3
#define TRIG_B 7
#define ECHO_A 2
#define ECHO_B 6
#define VCC_A 4
#define VCC_B 8
#define GND_B 5 
#define BOARD_LED 13

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BOARD_LED, OUTPUT);
  pinMode(VCC_A, OUTPUT);
  pinMode(VCC_B, OUTPUT);
  pinMode(GND_B, OUTPUT);
  
  setUltrasound(TRIG_A, ECHO_A);
  setUltrasound(TRIG_B, ECHO_B);
  
  digitalWrite(VCC_A, HIGH);
  digitalWrite(VCC_B, HIGH);
  digitalWrite(GND_B, LOW);
}

void setUltrasound(int trig, int echo) {
  
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT_PULLUP);
}

char* temp = new char[8];
uint8_t output[4];

uint32_t read(int trig, int echo) {
  
  uint32_t uCount = 0;
  // printRegisters(temp);
  
  digitalWrite(trig, LOW);
  digitalWrite(trig, HIGH);
  delayMicroseconds(20);
  digitalWrite(trig, LOW);
  
  while(digitalRead(echo) == 0) {};
  
  while(digitalRead(echo) == 1) {
    
    delayMicroseconds(1);
    uCount++;
  }
  
  return uCount;
}
  
void serialize(uint32_t u) {
  
  /*
  output[0] = '0';
  output[1] = '1';
  output[2] = '2';
  output[3] = '3';
  */
  output[0] = u % 0x100;
  output[1] = (u / 0x100) % 0x100;
  output[2] = (u / 0x10000) % 0x100;
  output[3] = (u / 0x1000000) % 0x100;
  
  Serial.write(output, 4);
}

uint32_t reading_A, reading_B;

void loop() {
  
  reading_A = read(TRIG_A, ECHO_A);
  reading_B = read(TRIG_B, ECHO_B);
  
  Serial.write('#');
  serialize(reading_A);
  serialize(reading_B);
  
  digitalWrite(BOARD_LED, 1);
  delay(25);

  digitalWrite(BOARD_LED, 0);
  delay(25);
}

void printRegisters(char* charBuffer /* 8 bytes minimum */) {

  Serial.print("B: ");
  byteAsString(charBuffer, PINB);
  Serial.print(charBuffer);
  Serial.print(" C: ");
  byteAsString(charBuffer, PINC);
  Serial.print(charBuffer);
  Serial.print(" D: ");
  byteAsString(charBuffer, PIND);
  Serial.print(charBuffer);
  Serial.println("");
}

void byteAsString(char* returnString, uint8_t b) {
  
  for(int i = 0; i < 8; i ++) {

    returnString[7 - i] = ((b >> i) & 1) > 0 ? '1' : '0';
  }
}
