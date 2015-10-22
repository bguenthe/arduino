// Graph
// by David A. Mellis
//
// Demonstrates reading data from the Arduino board by graphing the
// values received.
//
// based on Analog In 
// by <a href="http://itp.jtnimoy.com">Josh Nimoy</a>. 
// 
// Modified by Peter Koenig 

import processing.serial.*;

Serial port;
String buff = "";
int NEWLINE = 10;

// die letzten 128 Werte werden gespeichert
int[] xvalues = new int[128];
int[] yvalues = new int[128];
int[] zvalues = new int[128];

int xval, yval, zval, avx, avy, avz;
PFont font;

void setup()  
{
  size(512, 512);

  println("Available serial ports:");
  println(Serial.list());

  // Das Programm benutzt den ersten Port der Liste (Nummer 0).
  // Aendern Sie dies, falls der benutzte Port dort nicht
  // an erster Stelle steht, ggf. durch direkte Angabe des Ports:
  // port = new Serial(this, "COM1", 9600);
  // Der letzte Parameter (9600) gibt die Geschwindigkeit der 
  // Kommunikation an, er muss dem Wert entsprechen, der auf 
  // Arduino-Seite an Serial.begin() uebergeben wurde.

  port = new Serial(this, Serial.list()[0], 9600);  

   font = loadFont("Verdana-12.vlw");
   textFont(font); 
}

void draw()
{
  background(0);

  avx = 0;
  avy = 0;
  avz = 0;

  // zeichne die gespeicherten Werte und berechne dabei die 
  // Durchschnittswerte ueber die letzen 128 Messungen
  stroke(255, 0, 0); // x ist rot
  for (int i = 0; i < 127; i++){
    line(i * 4, 512 - xvalues[i]/2, (i + 1) * 4, 512 - xvalues[i + 1]/2);
    avx = avx + xvalues[i];
  }
  
  stroke(0, 255, 0); // y ist grün
  for (int i = 0; i < 127; i++){
    line(i * 4, 512 - yvalues[i]/2, (i + 1) * 4, 512 - yvalues[i + 1]/2);
    avy = avy + yvalues[i];
  }
  
  stroke(0, 0, 255); // z ist blau
  for (int i = 0; i < 127; i++){
    line(i * 4, 512 - zvalues[i]/2, (i + 1) * 4, 512 - zvalues[i + 1]/2);
    avz = avz + zvalues[i];
  }

  // gib die Durchschnittswerte aus
  fill (255, 0, 0);
  text("Average x value (last 128): "+ avx/128, 10, 20);
  fill (0, 255, 0);
  text("Average y value (last 128): "+ avy/128, 10, 40);
  fill (0, 0, 255);
  text("Average z value (last 128): "+ avz/128, 10, 60);

  while (port.available() > 0)
    serialEvent(port.read());
}

void serialEvent(int serial)
{
  if (serial != NEWLINE) {
    // speichere alle Zeichen einer Zeile
    buff += char(serial);
  } 
  else {
    if (buff.length() < 2) {
      buff = "";
      return;
    }
    // Das Zeilenende markieren ein carriage return und ein 
    // newline-Zeichen. Wegen des letzeren sind wir jetzt hier, 
    // aber der carriage return muss noch weg:
    buff = buff.substring(0, buff.length()-1);

    String bufs[] = buff.split(" ");
    if (bufs.length != 4) {
      buff = "";
      return;
    }

    // parse den gelesenen String als Integer 

    try {
      xval = Integer.parseInt(bufs[0]);
      yval = Integer.parseInt(bufs[1]);
      zval = Integer.parseInt(bufs[2]);
    }
    catch (NumberFormatException ex) {
      // manchmal kommt am Anfang Salat ueber die Leitung,  
      // der sich nicht als Integer parsen laesst.
      return;
    }

    // buff leer machen
    buff = "";

    // verschiebe die vorhandenen Werte, um Platz fuer
    // neue zu schaffen
    for (int i = 0; i < 127; i++)
      xvalues[i] = xvalues[i + 1];
    for (int i = 0; i < 127; i++)
      yvalues[i] = yvalues[i + 1];
    for (int i = 0; i < 127; i++)
      zvalues[i] = zvalues[i + 1];

    // schreibe die empfangenen Werte ins Array
    xvalues[127] = xval;
    yvalues[127] = yval;
    zvalues[127] = zval;
  }
}