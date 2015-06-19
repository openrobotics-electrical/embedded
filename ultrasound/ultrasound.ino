#define TRIG 3
#define ECHO 2
#define VCC_U1 4
#define BOARD_LED 13

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BOARD_LED, OUTPUT);
  pinMode(VCC_U1, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT_PULLUP);
  digitalWrite(VCC_U1, HIGH);
}

char* temp = new char[8];
uint8_t output[4];

void loop() {
  
  uint32_t uCount = 0;
  printRegisters(temp);
  
  digitalWrite(TRIG, LOW);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG, LOW);
  
  while(digitalRead(ECHO) == 0) {};
  
  while(digitalRead(ECHO) == 1) {
    
    delayMicroseconds(1);
    uCount++;
  }
  
  output[0] = uCount % 0x100;
  output[1] = (uCount / 0x100) % 0x100;
  output[2] = (uCount / 0x10000) % 0x100;
  output[3] = (uCount / 0x1000000) % 0x100;
  
  Serial.write('#');
  Serial.write(output, 4);
  
  digitalWrite(BOARD_LED, 1);
  delay(50);
  digitalWrite(BOARD_LED, 0);
  delay(50);
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
