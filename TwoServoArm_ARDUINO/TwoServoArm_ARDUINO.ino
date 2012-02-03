//arduino CODE

//----------------------------------------- parameter (CHANGE HERE)
#define UP_PARAMETER 0
#define DOWN_PARAMETER 100

//----------------------------------------- communication protocol (DON'T CHANGE!)
#define HEADER      '|' 
//command
#define MOUSE       'M' 
#define MOUSEUP     'U'
#define MOUSEDOWN   'D'
#define DEBUG       'N'
#define MEMORY_DATA 'A'

#define MESSAGE_BYTES 4 // HEADER - COMMAND

//----------------------------------------- coordinates, feel a bit stupid

prog_char path[] PROGMEM = {
  'D',0,0, 'M',50,70, 'M',51,70, 'M',52,70, 'M',53,70, 'M',54,70, 
  'M',55,70, 'M',56,70, 'M',57,70, 'M',58,70, 'M',59,70, 'M',60,70, 'M',61,70, 'M',62,70, 'M',63,70, 
  'M',64,70, 'M',65,70, 'M',66,70, 'M',67,70, 'M',68,70, 'M',69,70, 'M',70,70, 'M',71,70, 'M',72,70, 
  'M',73,70, 'M',74,70, 'M',75,70, 'M',76,70, 'M',77,70, 'M',78,70, 'M',79,70, 'M',80,70, 'M',81,70, 
  'M',82,70, 'M',83,70, 'M',84,70, 'M',85,70, 'M',86,70, 'M',87,70, 'M',88,70, 'M',89,70, 'U',0,0};
// how long is the path
#define PATH_LENGTH 126
//----------------------------------------- application
#include <Servo.h> 
#include <avr/pgmspace.h>

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

//----------------------------------------- loop
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

      //Serial.print(tag);

      decodeMessage(tag, a, b);
      //  delay(15);
    }
  }
}

void decodeMessage(char tag, unsigned char a, unsigned char b) {
  if     (tag == MOUSEDOWN) setPen(true);
  else if(tag == MOUSEUP) setPen(false);
  else if(tag == DEBUG) debugUP(a);
  else if(tag == MOUSE) moveArm(a,b); 
  else if(tag == MEMORY_DATA) loadMemoryAndDraw();
  else {
    Serial.print("got message with unknown tag "); 
    Serial.println(tag);
  }
}

//----------------------------------------- commands
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

  Serial.print("theta: "); 
  Serial.print(atheta); 
  Serial.print(", beta: "); 
  Serial.println(abeta);
  theta.write(atheta);
  beta.write(abeta);
}

void loadMemoryAndDraw() {
  Serial.println("---------");
  for(int i=0; i< PATH_LENGTH ; i+=3) {
    Serial.print(i);
    Serial.print("# ");

    char tag = pgm_read_byte(&path[i]);
    unsigned char a = pgm_read_byte(&path[i+1]);
    unsigned char b = pgm_read_byte(&path[i+2]);

    Serial.print(tag);
    Serial.print("-");
    Serial.print(a);
    Serial.print(",");
    Serial.println(b);
    
    decodeMessage(tag,a,b);
    delay(50);
  }
  Serial.println("---------");
}




