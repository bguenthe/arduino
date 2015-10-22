/*
 * Demoprogramm fuer den 3D-Spiele-Controller
 */

import ddf.minim.*;
import processing.serial.*;

Serial port;
Minim minim;
Lander lander;
Landscape landscape;

// bestimmt das Verhaeltnis zwischen Aenderungen der Sensordaten
// und Neigungswinkel der Landefaehre
final float SCALE = 50;

// Ersetzen Sie diese Durchschnittswerte durch jene, die Ihr
// Controller mit dem Programm ADXLgraph liefert:
final int X_AV = 342;
final int Y_AV = 351; 
final int Z_AV = 412;

// Diese Winkel geben an, wie die Sensorebenen gegenueber dem 
// Steckbrett verdreht sind. Die Vorgabewerte passen zur von 
// Segor vertriebenen Sensorplatine.
// Sind die Kanten des Sensors in allen drei Richtungen parallel
// zum Steckbrett, sind alle Winkel 0.
final float XY_ANG = PI/4.0;
final float XZ_ANG = 0.0;
final float YZ_ANG = 0.0;

// Die Drehung erfolgt ueber eine Rotationsmatrix, 
// die mit neun Werten Drehungen in allen Dimensionen
// widerspiegeln kann.
// Beschrieben wird die Rotation des Sensors gegenueber 
// dem Steckbrett.
// x-z bezeichnet die Spalten, 1-3 die Zeilen, sodass: 
// --> x1 die x-Komponente der x-Achse des Sensors bezogen auf die
// x-Achse des Sensors angibt; 
// --> x3 die z-Komponente der x-Achse des Sensors bezogen auf die
// x-Achse des Sensors angibt;
// usf.
// Wenn der Sensor nicht rotiert ist, sieht die Matrix wie folgt aus:
//   | x y z
// --+-------
// 1 | 1 0 0 
// 2 | 0 1 0 
// 3 | 0 0 1 
// Das entspricht der Eingabe von 0 bei allen drei Winkeln oben.

float x1 = cos(XY_ANG);
float x2 = sin(XY_ANG);
float x3 = 0;

float y1 = -sin(XY_ANG);
float y2 = cos(XY_ANG);
float y3 = 0;

float z1 = 0;
float z2 = 0;
float z3 = 1;

// rohe Sensor-Messwerte:
int x_raw, y_raw, z_raw; 
// umgerechnete Beschleunigungen parallel zu den
// Seiten des Steckbretts
float x, y, z; 

String buffer = "";
int NEWLINE = 10;
boolean ready = true;

// Setzen Sie dies auf true, sobald ein Arduino angeschlossen ist
boolean serialAvailable = true;

void setup() {
  minim = new Minim(this);
  size(1200, 360);
  colorMode(RGB, 255, 255, 255, 100);
  smooth();
  landscape = new Landscape();
  lander = new Lander(30, 30, landscape);

  if (serialAvailable) {
    println("Available serial ports:");
    println(Serial.list());
 
  // Das Programm benutzt den ersten Port der Liste (Nummer 0).
  // Aendern Sie dies, falls der benutzte Port dort nicht
  // an erster Stelle steht, ggf. durch direkte Angabe des Ports:
  // port = new Serial(this, "COM1", 9600);
  // Der letzte Parameter (9600) gibt die Geschwindigkeit der 
  // Kommunikation an, er muss dem Wert entsprechen, der auf 
  // Arduino-Seite an Serial.begin() uebergeben wurde.
 
    port = new Serial(this, "COM4", 9600);  
    println("Waiting for serial port ...");
    ready = false;
  }
}

void draw() {
  background(10, 10, 30);
  if (ready) {
    lander.run();
    landscape.run();
  }
  while (serialAvailable && port.available() > 0) {
    serialEvent(port.read());
  }
}

void keyPressed() {
  if (key == CODED) {
    if (keyCode == UP) {
      lander.rocketOn();
    } 
    else if (keyCode == LEFT) {
      lander.rotateLeft();
    } 
    else if (keyCode == RIGHT) {
      lander.rotateRight();
    }
  }  
  else if (key == 'r') {
    lander = new Lander(30, 30, landscape);  
  } 
}

void keyReleased() {
  if (key == CODED) {
    if (keyCode == UP) {
      lander.rocketOff();
    }
  }
}

void serialEvent(int serial) {
  if (serial != NEWLINE) {
    // speichere alle Zeichen einer Zeile
    buffer += char(serial);
  } 
  else {
    if (buffer.length() < 2) {
      buffer = "";
      return;
    }
// Das Zeilenende markieren ein carriage return und ein 
    // newline-Zeichen. Wegen des letzeren sind wir jetzt hier, 
    // aber der carriage return muss noch weg:
    buffer = buffer.substring(0, buffer.length()-1);
    
    String bufs[] = buffer.split(" ");
    if (bufs.length != 4) {
      buffer = "";
      return;
    }

    // parse den gelesenen String als Integer 

    try {
    x_raw = Integer.parseInt(bufs[0]) - X_AV;
    y_raw = Integer.parseInt(bufs[1]) - Y_AV;
    z_raw = Integer.parseInt(bufs[2]) - Z_AV;
    }
    catch (NumberFormatException ex) {
      // manchmal kommt am Anfang Salat ueber die Leitung,  
      // der sich nicht als Integer parsen laesst.
      return;
    }
    // Das Drehen der rohen Sensorwerte erfolgt durch Matrizenmultiplikation:
    // Die Matrix x_raw, y_raw, z_raw wird von rechts an die Drehmatrix multipliziert,
    // das Ergebnis sind die gedrehten Werte:
    x = x1 * x_raw + y1 * y_raw + z1 * z_raw;
    y = x2 * x_raw + y2 * y_raw + z2 * z_raw;
    z = x3 * x_raw + y3 * y_raw + z3 * z_raw;
    
    boolean buttonPressed = bufs[3].equals("1");

    buffer = "";

    if (buttonPressed) {
      lander.rocketOn();
    }
    else {
      lander.rocketOff();
    }
    
    lander.rotateAngle(-y/SCALE);
    ready = true;
  }
}


void stop() {
  lander.stop();
  minim.stop();
  super.stop();
}