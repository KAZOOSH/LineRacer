
//speed tracking
const int nBikes = 1;
const int speedPin[2] = {7,8};
const int lInputs = 3;

long lastSequence[2] = {0,0};
int tSequence = 500;
bool lastInput[2] = {true,true};
int inputs[2][3] = {{0,0,0},{0,0,0}};
int index[2] = {0,0};


void updateSpeed(int bike){
   if(millis()-lastSequence[bike] > tSequence){
    index[bike]++;
    index[bike]%=lInputs;
    inputs[bike][index[bike]] = 0;
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
    updateSpeed[i];
  }

  float speed[2] = {0,0};
  for(int s=0;s<2;++s){
    for(int i=0;i<lInputs;++i){
      speed[s] += inputs[s][i];
    }
    speed[s] /= (float)lInputs;
    speed[2] = (1000.0/speed[s])-1;
  }

  Serial.print(speed[0]);
  Serial.print( " > <");
  Serial.print(speed[1]);
  Serial.println();
    
  delay(10);        
}
