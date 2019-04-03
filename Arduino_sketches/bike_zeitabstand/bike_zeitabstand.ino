bool lastInput = true;
int rounds = 0;

long lastSequence = 0;
int tSequence = 500;


int inputs[3] = {0,0,0};
int index = 0;
int lInputs = 3;

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(8,INPUT_PULLUP);
}

// the loop routine runs over and over again forever:
void loop() {
  if(millis()-lastSequence > tSequence){
    rounds++; 
    index++;
    index%=lInputs;
    inputs[index] = 1000;
    lastSequence = millis();
  }
  // read the input on analog pin 0:
  int sensorValue = digitalRead(8);
  
  // print out the value you read:
  if(sensorValue==0 && lastInput == true){
    rounds++; 
    index++;
    index%=lInputs;
    inputs[index] = millis()-lastSequence;
    lastSequence = millis();
  }
  lastInput = sensorValue;

  float speed = 0;
  for(int i=0;i<lInputs;++i){
    speed += inputs[i];
  }
  speed /= (float)lInputs;
  speed = (1000.0/speed)-1;

  Serial.print(speed);
  Serial.print( " > ");
  for(int i=0; i<lInputs; ++i){
    Serial.print(" ");
    Serial.print(inputs[i]);
  }
  Serial.println();
    
  delay(10);        // delay in between reads for stability
}
