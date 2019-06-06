/*
  Example for different sending methods
  
  https://github.com/sui77/rc-switch/
  
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();
uint8_t serialBuffer[2];
uint8_t inByte = 0;  


void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
  
  // Transmitter is connected to Arduino Pin #10  
  mySwitch.setProtocol(5);
  //mySwitch.setRepeatTransmit(5);
  mySwitch.enableTransmit(10);

  // Optional set pulse length.
  // mySwitch.setPulseLength(320);
  
  // Optional set protocol (default is 1, will work for most outlets)
  // mySwitch.setProtocol(2);
  
  // Optional set number of transmission repetitions.
  // mySwitch.setRepeatTransmit(15);
  pinMode(LED_BUILTIN, OUTPUT);

  while (!Serial1.available()) {
    Serial1.print(1);
    delay(500);
    Serial.println("connect");
  }
  Serial.println("connected");
}

void loop() {

  //serial input
  while (Serial1.available() > 0)
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)  
    inByte = Serial1.read();
    if(inByte < 128) serialBuffer[0] = inByte;
    else serialBuffer[1] = inByte-128;
    
//    translate speeds to 8-bit integers
  uint16_t speedInt1 = ((uint8_t)serialBuffer[0]) & B01111111;
  uint16_t speedInt2 = ((uint8_t)serialBuffer[1]) & B01111111;
Serial.print(serialBuffer[0],DEC);
Serial.println(serialBuffer[1],DEC);

  // codeword = speed1 | speed2
  uint16_t codeword = ( speedInt1 << 8 ) | speedInt2;
  mySwitch.send( codeword, 16 );
  //delay(500);
  //Serial.print("sent value: ");
  //Serial.println( codeword );
  //delay(20);
 // Serial1.print((uint8_t)millis());
  }

  
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  //delay(); 

}
