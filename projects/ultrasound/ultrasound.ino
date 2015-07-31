#define TRIG_1 9
#define TRIG_2 7
#define TRIG_3 5
#define TRIG_4 3
#define ECHO_1 8
#define ECHO_2 6
#define ECHO_3 4
#define ECHO_4 2
#define BOARD_LED 13
#define TIMEOUT_uS 60600 // 10m both ways @ 330m/s sound velocity

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BOARD_LED, OUTPUT);
  
  setUltrasound(TRIG_1, ECHO_1);
  setUltrasound(TRIG_2, ECHO_2);
  setUltrasound(TRIG_3, ECHO_3);
  setUltrasound(TRIG_4, ECHO_4);
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
  delayMicroseconds(10);
  digitalWrite(trig, HIGH);
  delayMicroseconds(50);
  digitalWrite(trig, LOW);
  
  while(digitalRead(echo) == 0 && uCount < TIMEOUT_uS) {
    delayMicroseconds(1);
    uCount++;
  }
  
  uCount = 0;
  
  while(digitalRead(echo) == 1 && uCount < TIMEOUT_uS) {
    
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

uint32_t readings[4];

void loop() {
  
  readings[1] = read(TRIG_1, ECHO_1);
  readings[2] = read(TRIG_2, ECHO_2);
  
  /*
  Serial.write('#');
  serialize(reading_A);
  serialize(reading_B);
  */
  
  Serial.println(readings[1]);
  Serial.println(readings[2]);
  
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
