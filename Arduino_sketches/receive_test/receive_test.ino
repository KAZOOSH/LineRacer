/*
  Simple example for receiving
  
  https://github.com/sui77/rc-switch/
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  
  mySwitch.setProtocol(5);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
}

void loop() {
  if (mySwitch.available()) {
    
    uint16_t codeword = mySwitch.getReceivedValue();
    uint8_t speedInt1 = (uint8_t) codeword >> 8;
    uint8_t speedInt2 = (uint8_t) codeword;

    Serial.print("rec :"); Serial.println( codeword );



    mySwitch.resetAvailable();
  }
}
