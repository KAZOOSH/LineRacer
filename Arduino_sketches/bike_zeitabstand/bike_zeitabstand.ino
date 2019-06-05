#include <RCSwitch.h>

//rc
RCSwitch radio = RCSwitch();
const int rcPin = 10;
const unsigned int sendLength = 32; // bits
const int sendIntervalMillis = 250;
long lastSend = 0;

//serial
char serialBuffer[3];
float speedMult[2] = {1.0,1.0};
bool fixedSpeed[2] = {false,false};

//speed tracking
const int nBikes = 2;
const int speedPin[2] = {7,8};
const int lInputs = 3;
char buf[10];

long lastSequence[2] = {0,0};
int tSequence = 500;
int tMax = 1500;
bool lastInput[2] = {true,true};
int inputs[2][3] = {{tMax,tMax,tMax},{tMax,tMax,tMax}};
unsigned int index[2] = {0,0};
float speed[2] = {0.0,0.0};


void updateSpeed(int bike){
   if(millis()-lastSequence[bike] > tSequence){
    index[bike]++;
    index[bike]%=lInputs;
    inputs[bike][index[bike]] = tMax;
    lastSequence[bike] = millis();
  }
  // read the input on analog pin 0:
  int sensorValue = digitalRead(speedPin[bike]);

  // print out the value you read:
  if(sensorValue==0 && lastInput[bike] == true){
    index[bike]++;
    index[bike]%=lInputs;
    inputs[bike][index[bike]] = millis()-lastSequence[bike];
    lastSequence[bike] = millis();
  }
  lastInput[bike] = sensorValue;
}

void calculateSpeed(int bike){
  if(!fixedSpeed[bike]){
    speed[bike] = 0;
    for(int i=0;i<lInputs;++i){
      speed[bike] += inputs[bike][i];
    }
    speed[bike] /= (float)lInputs;
    speed[bike] = (1000.0/speed[bike])-1;
    if (speed[bike] < 0) speed[bike] = 0;
  }
}

// 3 Bytes: <mode> <bike> <value>
void evaluateSerialInput(){

  unsigned char bike = serialBuffer[1];
  unsigned char value = serialBuffer[2];

  if ( bike == '0' ) { bike = 0; }
  if ( bike == '1' ) { bike = 1; }

  switch(serialBuffer[0]){
    case 'm':{ //multiplicator
      float m = ((float)value)*0.00784;
      if(value == 255) m = 1.0;
      speedMult[bike] = m;
      Serial.print("#mult bike:");
      Serial.print((int)bike);
      Serial.print(" value:");
      Serial.print((int)value);
      Serial.print(" -> ");
      Serial.println(m);
      break;
    }
    case 's':{ //set speed
      float s = ((float)value)*0.03917647058;
      speed[bike] = s;
      Serial.print("#speed bike:");
      Serial.print((int)bike);
      Serial.print(" value:");
      Serial.print((int)value);
      Serial.print(" -> ");
      Serial.println(s);
      break;
    }
    case 'e':{ //enable fixed speed
      fixedSpeed[bike] = (bool)value;
      Serial.print("#fixedSpeed ");
      Serial.print((int)value);
      Serial.print(" -> ");
      Serial.println((bool)value);
      break;
    }
  }
}

void rcSendSpeed()
{
  // translate speeds to 8-bit integers
  uint16_t speedInt1 = ( (uint8_t) round(speed[0]) ) & B01111111;
  uint16_t speedInt2 = ( (uint8_t) round(speed[1]) ) & B01111111;

  // codeword = speed1 | speed2
  uint16_t codeword = ( speedInt1 << 8 ) | speedInt2;
  radio.send( codeword, sizeof(codeword) );

  Serial.print("sent value: ");
  Serial.println( codeword );
}


void setup() {
  Serial.begin(9600);
  radio.setProtocol(5);
  radio.setRepeatTransmit(3);
  radio.enableTransmit(rcPin);

  //init bikes
  for(int i=0; i<nBikes; ++i){
    pinMode(speedPin[i],INPUT_PULLUP);
  }
}

// the loop routine runs over and over again forever:
void loop() {

  //serial input
  while (Serial.available() > 0)
  {
    Serial.readBytes(serialBuffer, 3);
    evaluateSerialInput();
  }
/*
  //update bikes
  for(int i=0; i<nBikes;++i){
    updateSpeed(i);
    calculateSpeed(i);
  }
*/
  if(lastSend - millis() > sendIntervalMillis){
    digitalWrite( 13, LOW );
    Serial.print("send s0:");
    Serial.print( speed[0] );
    Serial.print( " s1:" );
    Serial.print( speed[1] );
    Serial.println();
    rcSendSpeed();
    lastSend = millis();
    digitalWrite( 13, HIGH );
  }
}
