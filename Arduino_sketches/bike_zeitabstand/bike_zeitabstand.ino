#include <RCSwitch.h>

//rc
RCSwitch mySwitch = RCSwitch();
const int rcPin = 10;
const int sendOption = 24;
const int sendIntervall = 1000;
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
float speed[2] = {0,0};


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

void evaluateSerialInput(){
  switch(serialBuffer[0]){
    case 'm':{ //multiplicator
      float m = (float)((int)serialBuffer[1])*0.00784;
      if((int)serialBuffer[1] == 127) m = 1.0;
      speedMult[(int)serialBuffer[1]] = m;
      Serial.print("#mult ");
      Serial.print((int)serialBuffer[1]);
      Serial.print(" -> ");
      Serial.println(m);
      break;
    }
    case 's':{ //set speed
      float s = (float)((int)serialBuffer[1])*0.03917647058;
      speed[(int)serialBuffer[1]] = s;
      Serial.print("#speed ");
      Serial.print((int)serialBuffer[1]);
      Serial.print(" -> ");
      Serial.println(s);
      break;
    }
    case 'e':{ //enable fixed speed
      fixedSpeed[(int)serialBuffer[1]] = (bool)serialBuffer[2];
      Serial.print("#fixedSpeed ");
      Serial.print((int)serialBuffer[1]);
      Serial.print(" -> ");
      Serial.println((bool)serialBuffer[2]);
      break;
    }
  }
}

void rcSendSpeed(){
  mySwitch.send((int)speed[0]*2, sendOption);
}


void setup() {
  Serial.begin(9600);
  mySwitch.enableTransmit(rcPin);

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

  //update bikes
  for(int i=0; i<nBikes;++i){
    updateSpeed(i);
    calculateSpeed(i);
  }

  if(lastSend - millis() > sendIntervall){
    Serial.println("send");
    Serial.print("s");
  dtostrf(speed[0], 3, 2, buf);
  Serial.print(buf);
  dtostrf(speed[1], 3, 2, buf);
  Serial.println(buf);
    rcSendSpeed();
    lastSend = millis();
  }
  

  //serial output speed
  //

 // delay(10);        
}
