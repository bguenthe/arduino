#include <Bounce2.h>

int tasterPin = 8;

int ledPins[] = {9,10,11,12,13};
boolean ledPinsStatus[] = {false,false,false,false,false};

int tasterStatus;
int lastStatus = LOW;
Bounce bouncer = Bounce();

const String device = "from_arduino"; // used for the serial messages

void setup() {
  for (int i = 0; i <= sizeof(ledPins); i++ ) {
    pinMode(ledPins[i], OUTPUT);
  }

  cycle();

  pinMode(tasterPin, INPUT);
  int tasterStatus = LOW;
  
  Serial.begin(9600);

  bouncer.attach(tasterPin);
  bouncer.interval(5);
}

void cycle() {
  for (int i = 0; i <= sizeof(ledPins); i++ ) {
    digitalWrite(ledPins[i], HIGH);
    delay(10);
    digitalWrite(ledPins[i], LOW);
  }
}

void loop() {
  bouncer.update();
  tasterStatus = bouncer.read();
  if (Serial.available() > 0) { // wenn eine Anfrage reinkommt
    String str = Serial.readStringUntil('\n');
    String ts;
    int mode;
    int pin;
    if (str == "get_status") {
      cycle();
      if (tasterStatus == LOW) {
        ts = "FALSE"; 
      } else {
        ts = "TRUE";
      }

      String message = "{\"button_status\"" + String(":") + String("\"") + ts + String("\"}");
      Serial.println(message);
    } else if (str == "cycle") {
      cycle();
    } else if (str.startsWith("/LED")) { // Beispiel: "/LED1/on"
      pin = str.substring(4,5).toInt();
      pin = pin - 1;
      if (str.endsWith("on")) {
        mode = HIGH;  
      } else {
        mode = LOW;
      }

      digitalWrite(ledPins[pin], mode);
    }
  }

  if (tasterStatus == LOW) {
    if (tasterStatus != lastStatus) {
      digitalWrite(ledPins[0], LOW);
      Serial.println("{\"button\":\"FALSE\"}");
    }
  } else {
    if (tasterStatus != lastStatus) {    
      digitalWrite(ledPins[0], HIGH);
      Serial.println("{\"button\":\"TRUE\"}");
    }
  }
  lastStatus = tasterStatus;
}

