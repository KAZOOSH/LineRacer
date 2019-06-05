/*
  Example for different sending methods
  
  https://github.com/sui77/rc-switch/
  
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();
char serialBuffer[2];


void setup() {

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
  
}

void loop() {

  //serial input
  while (Serial.available() > 0)
  {
    Serial.readBytes(serialBuffer, 2);
//    translate speeds to 8-bit integers
  uint16_t speedInt1 = (serialBuffer[0]) & B01111111;
  uint16_t speedInt2 = (serialBuffer[1]) & B01111111;


  // codeword = speed1 | speed2
  uint16_t codeword = ( speedInt1 << 8 ) | speedInt2;
  mySwitch.send( codeword, 16 );

  //Serial.print("sent value: ");
  //Serial.println( codeword );
  }

}
