#include <SparkFunTMP102.h>
#include <SparkFun_TB6612.h>

// include qtrSensors
#include <QTRSensors.h>

// motor stuff ----------------------------------------------------

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
// the default pins listed are the ones used on the Redbot (ROB-12097) with
// the exception of STBY which the Redbot controls with a physical switch
#define AIN1 2
#define BIN1 7
#define AIN2 4
#define BIN2 8
#define PWMA 5
#define PWMB 6
#define STBY 9

// these constants are used to allow you to make your motor configuration 
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

const int maximum_motor_speed = 255;

int player_speed = 0;

// maxiumum base speed for racer (will be affected by speed of ergometer)
int player_speed_max = 160;

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);
#define KP 2 //experiment to determine this, start by something small that just makes your bot follow the line at a slow speed
#define KD 5 //experiment to determine this, slowly increase the speeds and adjust this value. ( Note: Kp < Kd) 


// qr sensor stuff -------------------------------------------

#define MIDDLE_SENSOR 4       //number of middle sensor used
#define NUM_SENSORS 8         //number of sensors used
#define EMITTER_PIN 2

QTRSensors qtr;
unsigned int sensorValues[NUM_SENSORS];

void setup()
{
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A0, A1, A2, A3, A4, A5, A6, A7}, NUM_SENSORS);
  setPlayerSpeed(5);
  delay(1500);
  calibration();
}

int lastDistFromCenter = 0;
int lastLinePosition = 4;

int getLinePosition() {
  qtr.read(sensorValues);
  int maxvalue = 0;  
  int lineposition = -1;
  
  // loop sensor values, use values in upper third of min-max range for position 
  for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    if(sensorValues[i] > (qtr.calibrationOn.minimum[i] + (qtr.calibrationOn.maximum[i] - qtr.calibrationOn.minimum[i]) * 3/4)){
      //Serial.print(i);
      //Serial.print('\t');
      if(sensorValues[i] > maxvalue) {
        lineposition = i;
        maxvalue = sensorValues[i];
      }
      
    }
    //Serial.println(' ');
  }
  return lineposition;
}

void setPlayerSpeed(int speedValueFromCycle){
  player_speed = (int)(((float)speedValueFromCycle / 8.0) * (float)player_speed_max);
}

void loop()
{
   unsigned int sensors[NUM_SENSORS];

   // get lineposition with help of calibrated min max values
   int lineposition = getLinePosition();
   
   if(lineposition > -1) {
     // if lineposition was detected ( > -1 ) set last position 
     lastLinePosition = lineposition;
   }
   
   //int motorSpeed = KP * distFromCenter + KD * (distFromCenter - lastDistFromCenter);
   int motorSpeed =  (int)(40 * (lastLinePosition -4));


   // Todo: glätten der motorgeschwindigkeit, um schwingen zu mindern! 

   //Serial.print("motorSpeed ");
   //Serial.println(motorSpeed);
   //Serial.print("player_speed ");
   //Serial.println(player_speed);
   
   int leftMotorSpeed = player_speed + motorSpeed;
   int rightMotorSpeed = player_speed - motorSpeed;

   
   set_motors(leftMotorSpeed, rightMotorSpeed);
}

void set_motors(int motor1speed, int motor2speed){
  if(motor1speed <= 0) {
      motor1.brake();
    } else{
      if (motor1speed > maximum_motor_speed ) motor1speed = maximum_motor_speed;
      motor1.drive(motor1speed,0);
    }
  if(motor2speed <= 0) {
      motor2.brake();
  }else {
      if (motor2speed > maximum_motor_speed ) motor2speed = maximum_motor_speed;
      motor2.drive(motor2speed,0);
    }
}

void calibration(){ 
  // turn on led while manual calibration is on
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  // Call calibrate() 200 times to make calibration take about 10 seconds.
  for (uint16_t i = 0; i < 200; i++)
  {
    qtr.calibrate();
  }
  digitalWrite(LED_BUILTIN, LOW);

  // print the calibration minimum values measured when emitters were on
  Serial.begin(9600);
  for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtr.calibrationOn.minimum[i]);
    Serial.print(' ');
  }
  Serial.println();
    // print the calibration maximum values measured when emitters were on
  for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtr.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  delay(1000);
}

