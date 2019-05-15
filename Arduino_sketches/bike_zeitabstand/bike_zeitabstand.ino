
//speed tracking
const int nBikes = 2;
const int speedPin[2] = {7,8};
const int lInputs = 3;

long lastSequence[2] = {0,0};
int tSequence = 500;
int tMax = 1500;
bool lastInput[2] = {true,true};
int inputs[2][3] = {{tMax,tMax,tMax},{tMax,tMax,tMax}};
unsigned int index[2] = {0,0};
float maxSpeed[2] = {0,0};


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
  float speed = 0;
    for(int i=0;i<lInputs;++i){
      speed[bike] += inputs[s][i];
    }
    speed[bike] /= (float)lInputs;
    speed[bike] = (1000.0/speed[bike])-1;
    if(speed[bike] > maxSpeed[bike] && speed[bike] < 10) maxSpeed[bike] = speed[bike];
    else if (speed[bike] < 0) speed[bike] = 0;
}

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  //init bikes
  for(int i=0; i<nBikes; ++i){
    pinMode(speedPin[i],INPUT_PULLUP);
  }
}

// the loop routine runs over and over again forever:
void loop() {
  for(int i=0; i<nBikes;++i){
    updateSpeed(i);
  }

  float speed[2] = {0,0};
  for(int s=0;s<2;++s){
    for(int i=0;i<lInputs;++i){
      speed[s] += inputs[s][i];
    }
    speed[s] /= (float)lInputs;
    speed[s] = (1000.0/speed[s])-1;
    if(speed[s] > maxSpeed[s] && speed[s] < 10) maxSpeed[s] = speed[s];
    else if (speed[s] < 0) speed[s] = 0;
  }
  //Serial.print(maxSpeed[0]);
  //Serial.print(" <- ");
  //Serial.print(speed[0]);
  //Serial.print( " > < ");
  Serial.print(speed[1]);
  Serial.print(" -> ");
  for (int i=0;i<3;++i){
    Serial.print(inputs[1][i]);
    Serial.print(" ");
  }
  Serial.print(maxSpeed[1]);
  Serial.println();
    
  delay(10);        
}
