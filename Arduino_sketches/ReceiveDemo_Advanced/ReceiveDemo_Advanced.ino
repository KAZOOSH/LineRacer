/*
  Example for receiving
  
  https://github.com/sui77/rc-switch/
  
  If you want to visualize a telegram copy the raw data and 
  paste it into http://test.sui.li/oszi/
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
    output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength());

  uint16_t codeword = mySwitch.getReceivedValue();
  uint8_t speedInt11 = (uint16_t) codeword >> 8;
  uint8_t speedInt21 = (uint8_t) codeword;
  Serial.print("   ");
    Serial.print(speedInt11);
    Serial.print("   ");
    Serial.println(speedInt21);
    mySwitch.resetAvailable();
  }
}
