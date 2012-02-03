//arduino CODE

#define HEADER    '|' 
#define MOUSE     'M' 
#define MOUSEUP   'U'
#define MOUSEDOWN 'D'
#define DEBUG     'N'
#define MESSAGE_BYTES 5 // the total bytes in a message (??)

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
  getDrawFromProcessing();
}

void getDrawFromProcessing() {
  if ( Serial.available() >= MESSAGE_BYTES) {
    if( Serial.read() == HEADER) {
      Serial.print("got header. ");
      char tag = Serial.read(); 
      
      if(tag == MOUSEDOWN) {
        Serial.println("mouse DOWN. ");
        up.write(100);
      }
      else if(tag == MOUSEUP) {
        Serial.print("mouse UP. ");
        up.write(0);
      } 
      else if(tag == DEBUG) {
        unsigned char a = Serial.read(); // this was sent as a char but a char is [-127-127]. use unsigned char for [0,255]
        unsigned char b = Serial.read();
        
        Serial.print("DEBUG up-servo:  ");
        Serial.println(a);
        up.write(a);
      }
      else if(tag == MOUSE) {
        Serial.print("got mouse. ");
        unsigned char serialtheta = Serial.read(); // this was sent as a char but a char is [-127-127]. use unsigned char for [0,255]
        unsigned char serialbeta = Serial.read();
        
        serialtheta = constrain(serialtheta, 0, 180);
        serialbeta = constrain(serialbeta, 0, 180);
        
        setAngles(serialtheta, serialbeta);
      } 
      else {
        Serial.print("got message with unknown tag "); 
        Serial.println(tag);
      }
      //  delay(15);
    }
  }
}

void setAngles(int atheta, int abeta) {
  Serial.print("theta: ");
  Serial.print(atheta); 
  Serial.print(", beta: ");
  Serial.println(abeta);
  theta.write(atheta);
  beta.write(abeta);
}
