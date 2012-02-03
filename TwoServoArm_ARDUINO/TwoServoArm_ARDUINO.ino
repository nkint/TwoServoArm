//arduino CODE

//----------------------------------------- parameter (CHANGE HERE)
#define UP_PARAMETER 0
#define DOWN_PARAMETER 100

//----------------------------------------- communication protocol (DON'T CHANGE!)
#define HEADER    '|' 
//command
#define MOUSE     'M' 
#define MOUSEUP   'U'
#define MOUSEDOWN 'D'
#define DEBUG     'N'

#define MESSAGE_BYTES 4 // HEADER - COMMAND

//----------------------------------------- application
#include <Servo.h> 

Servo theta;
Servo beta;
Servo up;

const int buttonPin = 4;
int buttonState = 0;
int lastButtonState = 0;
int buttonPushCounter = 0;

void setup() {
  Serial.begin(115200);

  theta.attach(11);   
  beta.attach(10);
  up.attach(9);

  pinMode(buttonPin, INPUT);
}

void loop(){
  getDataFromProcessing();
}

void getDataFromProcessing() {
  if ( Serial.available() >= MESSAGE_BYTES) {
    if( Serial.read() == HEADER) {
      Serial.print("got header. ");
      char tag = Serial.read();
      unsigned char a = Serial.read(); // this was sent as a char but a char is [-127-127]. use unsigned char for [0,255]
      unsigned char b = Serial.read();

      if     (tag == MOUSEDOWN) setPen(true);
      else if(tag == MOUSEUP) setPen(false);
      else if(tag == DEBUG) debugUP(a);
      else if(tag == MOUSE) moveArm(a,b); 
      else {
        Serial.print("got message with unknown tag "); 
        Serial.println(tag);
      }
      //  delay(15);
    }
  }
}

void debugUP(int a) {
  Serial.print("DEBUG up-servo:  ");
  Serial.println(a);
  up.write(a);
}

void setPen(boolean flag) {
  if(flag) {
    Serial.println("got mouse DOWN. ");
    up.write(DOWN_PARAMETER);
  } 
  else {
    Serial.println("got mouse UP. ");
    up.write(UP_PARAMETER);
  }
}

void moveArm(int a, int b) 
{
  Serial.print("got mouse. ");
  unsigned char atheta = a;
  unsigned char abeta = b;

  atheta = constrain(atheta, 0, 180);
  abeta = constrain(abeta, 0, 180);

  Serial.print("theta: "); Serial.print(atheta); 
  Serial.print(", beta: "); Serial.println(abeta);
  theta.write(atheta);
  beta.write(abeta);
}



