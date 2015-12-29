#include <Bounce2.h>

int LEFT_REDLED_PIN = 10;
int LEFT_GREENLED_PIN = 11;

int RIGHT_REDLED_PIN = 12;
int RIGHT_GREENLED_PIN = 13;

int LEFT_BUTTON_PIN = 8; 
int RIGHT_BUTTON_PIN = 9;

//long action_time = 60*1000; // 1 Munute Laufzeit
//long stop_time = 30*1000; // 30 Sekunden beide Ampeln rot

long action_time = 5*1000; // 1 Munute Laufzeit
long stop_time = 2*1000; // 30 Sekunden beide Ampeln rot

bool action = false;
unsigned long runs = 0;

Bounce leftbouncer = Bounce();
Bounce rightbouncer = Bounce();

int lefttasterstatus;
int righttasterstatus;

unsigned long current_millis = 0;
unsigned long last_stop_millies = 0;
unsigned long last_action_millies = 0;

void setup() {
  pinMode(LEFT_REDLED_PIN, OUTPUT);
  pinMode(LEFT_GREENLED_PIN, OUTPUT);
  pinMode(RIGHT_REDLED_PIN, OUTPUT);
  pinMode(RIGHT_GREENLED_PIN, OUTPUT);

  pinMode(LEFT_BUTTON_PIN, INPUT);
  pinMode(RIGHT_BUTTON_PIN, INPUT);

  Serial.begin(9600);
  
  action = false;
  current_millis = 0;
  last_stop_millies = 0;
  last_action_millies = 0;
  runs = 0;

  leftbouncer.attach(LEFT_BUTTON_PIN);
  leftbouncer.interval(5);

  rightbouncer.attach(RIGHT_BUTTON_PIN);
  rightbouncer.interval(5);
}

void loop() {
  current_millis = millis();

  if (action == false) {
    if ((current_millis - last_stop_millies) < stop_time) {
      digitalWrite(LEFT_REDLED_PIN, HIGH);
      digitalWrite(LEFT_GREENLED_PIN, LOW);
      digitalWrite(RIGHT_REDLED_PIN, HIGH);
      digitalWrite(RIGHT_GREENLED_PIN, LOW);      
    } else {
      action = true;
      last_stop_millies = current_millis;
      last_action_millies = current_millis;
    }
  }

  if (action == true) {
    if ((current_millis - last_action_millies) < action_time) {
      if ((runs % 2) == 0) {
        digitalWrite(LEFT_REDLED_PIN, HIGH);
        digitalWrite(LEFT_GREENLED_PIN, LOW);
        digitalWrite(RIGHT_REDLED_PIN, LOW);
        digitalWrite(RIGHT_GREENLED_PIN, HIGH);
      } else {
        digitalWrite(LEFT_REDLED_PIN, LOW);
        digitalWrite(LEFT_GREENLED_PIN, HIGH);
        digitalWrite(RIGHT_REDLED_PIN, HIGH);
        digitalWrite(RIGHT_GREENLED_PIN, LOW);
      }
    } else {
      action = false;
      last_stop_millies = current_millis;
      last_action_millies = current_millis;
      runs++;
    }
  }

  leftbouncer.update();
  rightbouncer.update();

  lefttasterstatus = leftbouncer.read();
  righttasterstatus = rightbouncer.read();
}
