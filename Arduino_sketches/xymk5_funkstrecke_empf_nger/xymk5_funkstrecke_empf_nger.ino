/*
  Simple example for receiving
  
  https://github.com/sui77/rc-switch/
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
  mySwitch.setProtocol(5);
}

void loop() {
  if (mySwitch.available()) {
    
    uint16_t codeword = mySwitch.getReceivedValue();
    uint8_t speedInt1 = (uint8_t) codeword >> 8;
    uint8_t speedInt2 = (uint8_t) codeword;

    Serial.print("code:"); Serial.println( codeword );

   
    mySwitch.resetAvailable();
  }
}
