/*
  Example for different sending methods
  
  https://github.com/sui77/rc-switch/
  
*/

#include <RCSwitch.h>


const int buttonPinUp = 4;
const int buttonPinDown = 3;

int buttonStateDown = 0;
int buttonStateUp = 0; 

int sendSpeed = 0;

RCSwitch mySwitch = RCSwitch();

void setup() {

  Serial.begin(9600);
  
  // Transmitter is connected to Arduino Pin #10  
  mySwitch.enableTransmit(10);

  //buttons
   pinMode(buttonPinUp, INPUT_PULLUP);
   pinMode(buttonPinDown, INPUT_PULLUP);


   mySwitch.send(sendSpeed, 24);
    
  // Optional set protocol (default is 1, will work for most outlets)
  // mySwitch.setProtocol(2);

  // Optional set pulse length.
  // mySwitch.setPulseLength(320);
  
  // Optional set number of transmission repetitions.
  // mySwitch.setRepeatTransmit(15);
  
}

void loop() {

  int buttonStateUpNew = digitalRead(buttonPinUp);
  int buttonStateDownNew = digitalRead(buttonPinDown);


  
  if(buttonStateUpNew ==1 && buttonStateUp == 0) {
   if(sendSpeed < 8) {
    sendSpeed++;
    mySwitch.send(sendSpeed, 24);
    
    Serial.println(sendSpeed);
   }
  }

  if(buttonStateDownNew ==1 && buttonStateDown == 0) {
   if(sendSpeed > 0) {
    sendSpeed--;
    mySwitch.send(sendSpeed, 24);
    
    
    Serial.println(sendSpeed);
   }
  }
  buttonStateDown = buttonStateDownNew;
  buttonStateUp = buttonStateUpNew;

  /* Same switch as above, but using decimal code */
  delay(30);  
}
