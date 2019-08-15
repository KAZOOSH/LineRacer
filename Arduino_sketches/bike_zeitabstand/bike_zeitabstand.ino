
//rc
const int sendIntervalMillis = 1000;
long lastSend = 0;

//serial
char serialBuffer[3];
float speedMult[2] = {1.0,1.0};
bool fixedSpeed[2] = {false,false};

//speed tracking
const int nBikes = 2;
const int speedPin[2] = {7,8};
const int lInputs = 1;
char buf[10];

char rcBuf[2];
long lastSequence[2] = {0,0};
int tSequence = 600;
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
    float lastSpeed = speed[bike];
    speed[bike] = 0;
    for(int i=0;i<lInputs;++i){
      speed[bike] += inputs[bike][i];
    }
    speed[bike] /= (float)lInputs;
    speed[bike] = (1000.0/speed[bike])-1;
    speed[bike]*=speedMult[bike];
    if(speed[bike]>lastSpeed + 0.003)speed[bike] = lastSpeed+0.003;
    if (speed[bike] > 0) speed[bike]+=1;
    if (speed[bike] < 0) speed[bike] = 0;
    if (speed[bike] > 10) speed[bike] = 10;
    //if(lastSpeed > speed[bike] + 2)speed[bike] = lastSpeed+2;
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
    case 'b':{ //enable fixed speed
      fixedSpeed[0] = (bool)value;
      fixedSpeed[1] = (bool)value;
      speed[0] = 0;
      speed[1] = 0;
      Serial.print("#fixedSpeedBoth ");
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
 // uint16_t speedInt1 = ( (uint8_t) round(speed[0]) ) & B01111111;
 // uint16_t speedInt2 = ( (uint8_t) round(speed[1]) ) & B01111111;
// uint16_t speedInt1 = ( (uint8_t) (10) ) & B01111111;
// uint16_t speedInt2 = ( (uint8_t) (33) ) & B01111111;

  // codeword = speed1 | speed2
 // uint16_t codeword = ( speedInt1 << 8 ) | speedInt2;
  //radio.send( codeword, sizeof(codeword) );
  //uint8_t speed1 = (speed[0]*128)/10;
  //uint8_t speed2 = (speed[0]*128)/10;
  //Serial1.print(speed1);
  rcBuf[0] = (speed[0]*127)/10+1;
  rcBuf[1] = (speed[1]*127)/10+1 +128;
 // Serial.print(rcBuf[0],DEC);
 // Serial.println(rcBuf[1],DEC);
  Serial1.write(rcBuf[0]);
  Serial1.write(rcBuf[1]);
}


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  //init bikes
  for(int i=0; i<nBikes; ++i){
    pinMode(speedPin[i],INPUT_PULLUP);
  }
}

// the loop routine runs over and over again forever:
void loop() {

  //serial input
  
  int serialInput = Serial.read();

  if ( serialInput >= 0 )
  {
    unsigned char serialByte = (unsigned char) serialInput;
    
    if ( serialByte == 'm' || serialByte == 's' || serialByte == 'e' || serialByte == 'b' )
    {
      serialBuffer[0] = serialByte;
      serialBuffer[1] = Serial.read();
      serialBuffer[2] = Serial.read();

      evaluateSerialInput();
      lastSequence[0] = millis();
      lastSequence[1] = millis();
    }
  }

  /*while (Serial.available() >= 0)
  {
    serialBuffer[0] =
    Serial.readBytes(serialBuffer, 3);
    evaluateSerialInput();
    lastSequence[0] = millis();
    lastSequence[1] = millis();
  }*/

  //update bikes
  for(int i=0; i<nBikes;++i){
    updateSpeed(i);
    calculateSpeed(i);
  }

 /* while (Serial1.available() > 0)
  {
    int r = Serial1.read();
    if(r!= 0){ 
      rcSendSpeed();
      //Serial.println(r);
    }
  }*/

  // if time to send or if got clear-to-send from sender
  if( millis() - lastSend > sendIntervalMillis || Serial1.read() == 'K' ){
    rcSendSpeed();
    
    digitalWrite( 13, LOW );
    Serial.print("s");
    Serial.print( speed[0],3 );
    //Serial.print( " s1:" );
    Serial.print( speed[1],3 );
    Serial.println();
    
    lastSend = millis();
    digitalWrite( 13, HIGH );
  }
}
