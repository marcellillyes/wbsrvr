#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #1 (M1 and M2)
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 1);

//Downsampling variables
unsigned long previousMillis = 0;
unsigned long currentMillis;
unsigned long interval=50;
int c=0;
String command="step";
String ss;
String inputString="";
bool flag = true;

void setup() {
  Serial.begin(115200);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");
  
  //PWM did not work
  AFMS.begin(5000);  // create with the default frequency 1.6KHz 
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  myMotor->setSpeed(255);  // 10 rpm   
  
}

void loop() {

  //Frequency Test
 currentMillis=millis();

  if (abs(currentMillis - previousMillis) >= interval & command=="step") {
  
  previousMillis = currentMillis;
 
  myMotor->step(5, BACKWARD, DOUBLE);
  
  if(flag){
  myMotor->step(10, FORWARD, DOUBLE);
  flag= !flag;
  }
    
  else{
  myMotor->step(10, BACKWARD, DOUBLE);
  flag= !flag;
    }
      
   //Plant validation
   // myMotor->step(200, BACKWARD, DOUBLE); 
  }
}
