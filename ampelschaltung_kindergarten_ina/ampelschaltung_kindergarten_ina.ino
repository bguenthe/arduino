#include <Bounce2.h>

int LEFT_REDLED_PIN = 10;
int LEFT_GREENLED_PIN = 11;

int RIGHT_REDLED_PIN = 12;
int RIGHT_GREENLED_PIN = 13;

int LEFT_BUTTON_PIN = 8; 
int RIGHT_BUTTON_PIN = 9;

Bounce leftbouncer = Bounce();
Bounce rightbouncer = Bounce();

int lefttasterstatus;
int righttasterstatus;

void setup() {
  pinMode(LEFT_REDLED_PIN, OUTPUT);
  pinMode(LEFT_GREENLED_PIN, OUTPUT);
  pinMode(RIGHT_REDLED_PIN, OUTPUT);
  pinMode(RIGHT_GREENLED_PIN, OUTPUT);

  pinMode(LEFT_BUTTON_PIN, INPUT);
  pinMode(RIGHT_BUTTON_PIN, INPUT);

  Serial.begin(9600);

  leftbouncer.attach(LEFT_BUTTON_PIN);
  leftbouncer.interval(5);

  rightbouncer.attach(RIGHT_BUTTON_PIN);
  rightbouncer.interval(5);

}

void loop() {
  leftbouncer.update();
  rightbouncer.update();
    
  lefttasterstatus = leftbouncer.read();
  righttasterstatus = rightbouncer.read();

  if (lefttasterstatus == HIGH){
    digitalWrite(LEFT_REDLED_PIN, HIGH);
    digitalWrite(LEFT_GREENLED_PIN, HIGH);
  } else {
    digitalWrite(LEFT_REDLED_PIN, LOW);
    digitalWrite(LEFT_GREENLED_PIN, LOW);    
  }

  if (righttasterstatus == HIGH){
    digitalWrite(RIGHT_REDLED_PIN, HIGH);
    digitalWrite(RIGHT_GREENLED_PIN, HIGH);
  } else {
    digitalWrite(RIGHT_REDLED_PIN, LOW);
    digitalWrite(RIGHT_GREENLED_PIN, LOW);    
  }

}
