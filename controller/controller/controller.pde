/*
 * Siehe auch:
 * http://www.arduino.cc/en/Tutorial/ADXL3xx
 */

// Definition der Pins des Beschleunigungssensors ADXL330
int xPin = 2;                     // X-Achse
int yPin = 1;                     // Y-Achse
int zPin = 0;                     // Z-Achse
int buttonPin = 2;                // Schalter

void setup() {                    // Initialisierung
  Serial.begin(9600);             // Serielle Verb., 9600 baud 
  pinMode(buttonPin, INPUT);      // Schalter-Pin als Eingang
}

void loop() {
  Serial.print(analogRead(xPin)); // X-Achse lesen, seriell ausg.
  Serial.print(" ");
  Serial.print(analogRead(yPin)); // Y-Achse
  Serial.print(" ");
  Serial.print(analogRead(zPin)); // Z-Achse
  Serial.print(" ");
  Serial.print(digitalRead(buttonPin));  // Schalter
  Serial.println();               // Zeilenvorschub
  delay(50);                      // 50 ms warten 
}