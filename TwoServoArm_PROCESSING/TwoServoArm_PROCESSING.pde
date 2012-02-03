//processing CODE

float sx, sy, ex, ey, hx, hy, hxo, hyo;
float armLength;
float a, b;

// i due angoli della cinematica inversa
float theta, beta;
// i due angoli da mandare ad USB_WIREDino
int atheta, abeta;
int debug_atheta, debug_abeta, debug_pen=0;

int px=0, py=0;
PGraphics drawLayer;

import processing.serial.*;
Serial myPort;

public static final char HEADER = '|'; 
public static final char MOUSE = 'M';
public static final char MOUSEUP = 'U';
public static final char MOUSEDOWN = 'D';
public static final char DEBUG = 'N';

boolean premouse = false;
boolean  correct_of_workspace = false;

PrintWriter outputFile = null;

//import java.util.HashSet;
//HashSet points_set;

void setup() {
  size(500, 500);
  background(255, 224, 150);

  sx = width/2;
  sy = height/2;
  armLength = int(width/5);
  a = armLength;
  b = armLength;

  drawLayer = createGraphics(width, height, P2D);

  try{
    String portName = Serial.list()[0]; 
    myPort = new Serial(this, portName, 115200);
  } catch(Exception e) {
    println("### USB NOT WIRED"); 
  }
}

//---------------------------------------------------------------------------------- draw
//---------------------------------------------------------------------------------- draw
//---------------------------------------------------------------------------------- draw

void draw() {
  background(255);
  inverseKinematics(mouseX-sx, mouseY-sy );

  drawArms();
  drawText();
  drawHelp();

  updateDrawLayer();

  image(drawLayer, 0, 0);

  px = mouseX;
  py = mouseY;
  if (mousePressed) sendMessage(MOUSE, atheta, abeta);
  
  // detect mouse change state and send message
  if (mousePressed && !premouse){
    //println("DOWN");
    sendMessage(MOUSEDOWN, atheta, abeta);
  }
  if (!mousePressed && premouse) {
    //println("UP");
    sendMessage(MOUSEUP, atheta, abeta);
    
  }
  premouse=mousePressed;
}

void drawHelp() {
  fill(0);
  String s = "Press:\n"+
             "\t space : clear current draw\n"+
             "\n"+
             "\t 0    \t: both servos to zero\n"+
             "\t T-t  \t: increment-decrement servo theta\n"+
             "\t B-b \t: increment-decrement servo beta\n"+
             "\t N-n  \t: increment-decrement servo pen\n"+
             "\n"+
             "\t o \t: open file"+"design.txt" +"\n"+
             "\t c \t: close file\n"+
             "\t l \t: load and send all file content"+
             "\n";
  text(s, 10, 300);
  
  if(outputFile!=null) {
    fill(255,0,0);
    text("### RECORDING ###", 10, 10);
  }
}

void drawArms() {
  stroke(255, 0, 0, 100);
  fill(240, 0, 0, 200);
  ellipse(sx, sy, 10, 10);
  ellipse(ex, ey, 8, 8);
  ellipse(hx, hy, 6, 6);
  stroke(0);
  line(sx, sy, ex, ey);
  line(ex, ey, hx, hy);
}

void drawText() {
  fill(0);
  text("theta = "+int(atheta), 10, 20);
  text("beta = "+int(abeta), 10, 40);
}

void updateDrawLayer() {
  // if mouse is out of arm
  boolean f1 = mouseX>=hx-5 && mouseX<=hx+5 && mouseY>=hy-5 && mouseY<=hy+5;
  // if angles are in [0-180]
  boolean f2 = (atheta>=0 && atheta<=180)   &&   (abeta>=0 && abeta<=180);

  if (f1 && f2) {
    correct_of_workspace = true;
    
    drawLayer.beginDraw();
    if (mousePressed) drawLayer.stroke(255, 0, 0);
    else drawLayer.noStroke();
    drawLayer.line(px, py, mouseX, mouseY);
    drawLayer.endDraw();
  } 
  else {
    correct_of_workspace = false;
    
    fill(100, 100);
    rect(0, 0, width, height);
  }
}

//---------------------------------------------------------------------------------- computations
//---------------------------------------------------------------------------------- computations
//---------------------------------------------------------------------------------- computations

void inverseKinematics(float x, float y) {
  /*
 take x and y in sx-sy centered coordinate
   and set global vars:
   - ex, ey : first joing
   - hx, hy : second joint
   - atheta, btheta
   */

  doInverseKinematicsTrigonometry(x, y);
  computeArms();
  getDataToSend();
}

void doInverseKinematicsTrigonometry(float x, float y) {
  // http://www.openprocessing.org/visuals/?visualID=553

  float D, E, B, C;

  float dx = x;
  float dy = y;
  float distance = sqrt(dx*dx+dy*dy);

  float c = min(distance, a + b);

  B = acos((b*b-a*a-c*c)/(-2*a*c));
  C = acos((c*c-a*a-b*b)/(-2*a*b));

  D = atan2(dy, dx);
  E = D + B + PI + C;

  // hei, trigonometria, che cosa vuoi dirmi?

  theta = E;
  beta = D+B;

  //  // http://www.alvarolopes.com/resources/USB_WIREDino-arm-inverse-kinematics.png
  //  float d, beta, sigma, mu, fi;
  //  y *= -1;
  //
  //  println("x " + x);
  //  println("y " + y);
  //  d = sqrt(x*x + y*y);
  //
  //  beta = acos((a*a + b*b - d*d)/(2*a*b));
  //  println("beta "+beta);
  //
  //  sigma = acos((a*a + d*d -b*b)/(2*d*a));
  //  println("sigma "+sigma);
  //
  //  mu = acos((d*d + y*y - x*x)/(2*d*y));
  //  println("mu "+mu);
  //
  //  fi = (PI/2) - mu - sigma;
  //  println("fi "+fi);
  //
  //  this.theta = fi;
  //  this.beta = beta;
}

void computeArms() {
  ex = cos(theta)*a + sx;
  ey = sin(theta)*a + sy;

  hx = cos(beta)*b + ex; // ? perchè funziona sta cosa?
  hy = sin(beta)*b + ey;
}

//---------------------------------------------------------------------------------- arduino
//---------------------------------------------------------------------------------- arduino
//---------------------------------------------------------------------------------- arduino

void serialEvent(Serial p) { 
  // handle incoming serial data 
  String inString = myPort.readStringUntil('\n');
  if (inString != null) {
    print("ARDUINO ECHO ### ");
    println( inString );    // echo text string from arduino
  }
}

void sendMessage(char tag, int atheta, int abeta) {
  //println("send message "+atheta+" "+abeta);
  if(!correct_of_workspace) {
    println("OUT OF AVAIABLE WORKSPACE");
    return;  
  }
  
  try {
    myPort.write(HEADER); 
    myPort.write(tag);
    myPort.write(atheta); 
    myPort.write(abeta);
  } catch(Exception e) {
    //println("###USB not wired"); 
  }

  if (outputFile != null) {
    outputFile.println(int(atheta) + "," + int(abeta));
  }
}

void getDataToSend() {
  atheta = (int)processing2costantinoTHETA(this.theta);

  PVector v1 = new PVector(sx - ex, sy - ey);
  PVector v2 = new PVector(ex - hx, ey - hy);

  abeta = (int)(180 - degrees(PVector.angleBetween(v1, v2)));
}

//---------------------------------------------------------------------------------- utils
//---------------------------------------------------------------------------------- utils
//---------------------------------------------------------------------------------- utils

float processing2costantinoTHETA(float t) {
  return (345-degrees(t));
}

void keyPressed() {
  //println("keypressed "+key);

  if (key=='o' || key=='c')
    handleFile();

  if (key=='l')
    loadAndSend();

  if (key=='T' || key=='t' || key=='B' || key=='b' || key=='0' || key=='N' || key=='n')
    testAngles();

  if(key==' ')clear();
}

void clear(){
   drawLayer = createGraphics(width, height, P2D);
   if (outputFile!=null) {
     println("clear, close and open file again");
     outputFile.flush(); // Writes the remaining data to the file
     outputFile.close(); // Finishes the file
     outputFile = createWriter("design.txt");
   }
}

void handleFile() {
  if (key=='o' && outputFile==null) {
    println("open file"+ "design.txt" );
    outputFile = createWriter("design.txt");
  }
  if (key=='c' && outputFile!=null) {
    println("close file"+"design.txt");
    outputFile.flush(); // Writes the remaining data to the file
    outputFile.close(); // Finishes the file
    outputFile = null;
  }
}

void loadAndSend() {
  
  if(outputFile!=null) {
    outputFile.flush(); // Writes the remaining data to the file
    outputFile.close(); // Finishes the file
    outputFile = null;
  }
  
  try {
    BufferedReader reader = createReader("design.txt");
    String line;
    while ((line = reader.readLine()) != null) {

      String pts[] = split(line, ','); 
      int x = int(pts[0]);
      int y = int(pts[1]);
      sendMessage(MOUSE, x, y);

      // wait 40 milliseconds
      try{ Thread.sleep(40); } catch(Exception e) {println(e);}
    }//endwhile
  }
  catch (Exception e) {e.printStackTrace();}
}

void testAngles() {
  if (key=='N') {
    debug_pen++;
    debug_pen = constrain(debug_pen, 0,180);
    sendMessage(DEBUG, debug_pen, 0);
    return;
  }
  if (key=='n') {
    debug_pen--;
    debug_pen = constrain(debug_pen, 0,180);
    sendMessage(DEBUG, debug_pen, 0);
    return;
  }
  
  if (key=='T') debug_atheta++;
  if (key=='t') debug_atheta--;
  if (key=='B') debug_abeta++;
  if (key=='b') debug_abeta--;
  if (key=='0') debug_abeta= debug_atheta = 0;
  debug_atheta = constrain(debug_atheta, 0,180);
  debug_abeta = constrain(debug_abeta, 0, 180);
  sendMessage(MOUSE, debug_atheta, debug_abeta);
}
