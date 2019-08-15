/*
  Example for different sending methods
  
  https://github.com/sui77/rc-switch/
  
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();
uint8_t serialBuffer[2];
uint8_t inByte = 0;  


void setup() {

  Serial.begin(9600);
  
  // Transmitter is connected to Arduino Pin #10  
  mySwitch.setProtocol(2);
  mySwitch.setRepeatTransmit(5);
  mySwitch.enableTransmit(10);

  pinMode(LED_BUILTIN, OUTPUT);
}

void assignByte( unsigned char inByte )
{
  if(inByte < 128) serialBuffer[0] = inByte;
  else serialBuffer[1] = inByte-128;
}

int countOnes( uint16_t codeword )
{
  int numberOfOnes = 0;
  for ( int i = 0; i < 16; i++ ) { numberOfOnes += bitRead( codeword, i ); }
  return numberOfOnes;
}

void loop() {

  //serial input
  if ( Serial.available() >= 2 )
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)  
    assignByte( Serial.read() );
    assignByte( Serial.read() );
    
    // translate speeds to 8-bit integers
    uint16_t speedInt1 = (serialBuffer[0]) & B01111111;
    uint16_t speedInt2 = (serialBuffer[1]) & B01111111;
  
    // codeword = speed1 | speed2
    uint16_t codeword = ( speedInt1 << 8 ) | speedInt2;

    // compute parity bit for even parity
    uint8_t parityBit = countOnes( codeword ) % 2;

    // set parity bit twice for redundancy
    codeword |= parityBit << 15;
    codeword |= parityBit << 7;
    
    mySwitch.send( codeword, 16 );
  
    //Serial.print("sent value: ");
    //Serial.println( codeword );

    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(20);
    Serial.print( 'K' );
  }
}
