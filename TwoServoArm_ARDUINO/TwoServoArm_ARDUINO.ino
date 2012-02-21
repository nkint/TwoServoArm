//arduino CODE

//----------------------------------------- parameter (CHANGE HERE)
#define UP_PARAMETER 0
#define DOWN_PARAMETER 100

#define SERVO_1    11
#define SERVO_2    10
#define SERVO_3    9
#define LEDPIN     8
#define BUTTONPIN  7

#define LED_BLINK_INTERVAL 1000

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
  'M',103,95,'U',0,0,'M',103,95,'M',103,95,'M',103,95,'M',103,95,'M',
  103,95,'M',103,95,'M',103,95,'M',101,94,'M',100,93,'M',99,92,'M',98,91,'M',98,90,'M',97,89,'M',
  96,87,'M',95,85,'M',95,85,'M',94,85,'M',94,84,'M',93,82,'M',92,82,'M',92,82,'M',90,79,'M',89,78,
  'M',89,78,'M',88,77,'M',88,75,'M',87,75,'M',86,73,'M',85,73,'M',85,73,'M',85,72,'M',84,70,'M',84,
  70,'M',83,69,'M',82,68,'M',82,67,'M',82,67,'M',82,66,'M',81,66,'M',81,65,'M',81,64,'M',81,63,'M',
  81,62,'M',80,60,'M',80,60,'M',80,58,'M',80,57,'M',80,56,'M',80,55,'M',80,54,'M',81,53,'M',81,53,
  'M',81,53,'M',81,53,'M',82,52,'M',82,51,'M',82,51,'M',82,50,'M',83,50,'M',84,50,'M',84,50,'M',85,
  50,'M',85,49,'M',87,49,'M',87,48,'M',89,48,'M',90,48,'M',91,48,'M',92,48,'M',93,48,'M',94,48,'M',
  96,48,'M',97,49,'M',97,49,'M',99,50,'M',100,50,'M',100,51,'M',101,51,'M',102,51,'M',103,52,'M',
  104,53,'M',105,54,'M',106,54,'M',107,55,'M',107,56,'M',108,56,'M',108,57,'M',108,57,'M',110,59,'M',
  111,59,'M',111,60,'M',113,62,'M',114,63,'M',114,64,'M',114,64,'M',116,66,'M',116,67,'M',117,67,
  'M',117,68,'M',117,68,'M',117,68,'M',118,70,'M',118,70,'M',118,70,'M',118,71,'M',118,72,'M',118,
  72,'M',118,73,'M',119,74,'M',119,75,'M',119,75,'M',119,77,'M',120,78,'M',120,79,'M',120,80,'M',
  120,81,'M',121,81,'M',121,83,'M',120,82,'M',121,84,'M',121,84,'M',121,86,'M',121,87,'M',121,88,'M',
  121,91,'M',121,91,'M',121,93,'M',121,94,'M',120,94,'M',120,96,'M',120,96,'M',120,95,'M',119,96,'M',
  118,96,'M',118,96,'M',118,97,'M',117,97,'M',117,97,'M',117,96,'M',117,97,'M',116,98,'M',115,98,'M',
  115,98,'M',115,98,'M',114,98,'M',113,98,'M',112,98,'M',112,98,'M',111,98,'M',110,98,'M',108,97,'M',
  107,97,'M',107,97,'M',106,97,'M',106,97,'M',105,97,'M',104,97,'M',104,97,'M',104,97,'M',104,96,'M',
  104,96,'M',104,96,'M',104,96,'M',104,96,'M',103,96,'M',103,95,'M',103,95,'M',103,95,'M',103,95,'M',
  103,95,'M',103,95,'M',103,95,'M',103,96,'M',103,96,'D',0,0
};
// how long is the path
#define PATH_LENGTH 531
//----------------------------------------- application
#include <Servo.h> 
#include <avr/pgmspace.h>

boolean drawing = false;

Servo theta;
Servo beta;
Servo up;

// detect button change
const int buttonPin = BUTTONPIN;
int buttonState = 0;
int lastButtonState = 0;
int buttonPushCounter = 0;

// led blinking withoud delay
int ledState = LOW;
long previousMillis = 0;
long interval = LED_BLINK_INTERVAL;

void setup() {
  Serial.begin(115200);

  theta.attach(SERVO_1);   
  beta.attach(SERVO_2);
  up.attach(SERVO_3);

  pinMode(buttonPin, INPUT);
  pinMode(LEDPIN, OUTPUT);
}

//----------------------------------------- loop
void loop(){
  readButton();
  getDataFromProcessing();

  // led blinking
  if(!drawing) {
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;   
      if (ledState == LOW)
        ledState = HIGH;
      else
        ledState = LOW;
      digitalWrite(LEDPIN, ledState);
    }
  }

}

void readButton() {
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      // if button pressed.. nothing
    } 
    else {
      // if button released.. load and draw!
      loadMemoryAndDraw(); 
    }
  }
  lastButtonState = buttonState;
}

void getDataFromProcessing() {
  if ( Serial.available() >= MESSAGE_BYTES) {
    if( Serial.read() == HEADER) {
      Serial.print("got header. ");

      setDrawing(true);

      char tag = Serial.read();
      unsigned char a = Serial.read(); // this was sent as a char but a char is [-127-127]. use unsigned char for [0,255]
      unsigned char b = Serial.read();

      //Serial.print(tag);

      decodeMessage(tag, a, b);
      //  delay(15);
    } 
    else {
      setDrawing(false); 
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

void setDrawing(boolean state) {
  drawing = state;
  if (drawing) {
    digitalWrite(LEDPIN, HIGH);
  } 
  else {
    digitalWrite(LEDPIN, LOW);
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

  setDrawing(true);

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

  setDrawing(false);

}





