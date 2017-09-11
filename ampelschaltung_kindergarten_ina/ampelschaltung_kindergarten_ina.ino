#include <Bounce2.h>

int LEFT_REDLED_PIN = 10;
int LEFT_GREENLED_PIN = 11;

int RIGHT_REDLED_PIN = 12;
int RIGHT_GREENLED_PIN = 13;

int LEFT_BUTTON_PIN = 8;
int RIGHT_BUTTON_PIN = 9;

//long action_time = 60*1000; // 1 Munute Laufzeit
//long stop_time = 30*1000; // 30 Sekunden beide Ampeln rot
//long wait_button_time = 30*1000; // 30 Sekunden bis zum nächsten force-switch

long action_time = 5 * 1000;
long stop_time = 2 * 1000;
long wait_button_time = 10 * 1000;

unsigned long current_millis = 0;
unsigned long last_stop_millies = 0;
unsigned long last_action_millies = 0;
unsigned long last_wait_button_millis = 0;

bool action = false;
unsigned long runs = 0;

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

  action = false;
  current_millis = 0;
  last_stop_millies = 0;
  last_action_millies = 0;
  last_wait_button_millis = 0;
  runs = 0;

  leftbouncer.attach(LEFT_BUTTON_PIN);
  leftbouncer.interval(5);

  rightbouncer.attach(RIGHT_BUTTON_PIN);
  rightbouncer.interval(5);
}

void loop() {
  current_millis = millis();

  /* nach dem anschalten:
   *  1.) stop_time beide ampeln rot
   *  2.) action_time die linke ampel grün und rechte rot
   *  3.) stop_time beide ampeln rot
   *  4.) action_time die rechte ampel grün und linke rot
   *  5.) goto 1.)
  */

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

  /* tastercode */
  /* drücken eines tasters schaltet die korrepondierende ampel sofort auf grün,
   *  aber nur wenn das letzte drücken mindestens wait_time_button her ist
  */
  leftbouncer.update();
  rightbouncer.update();

  lefttasterstatus = leftbouncer.read();

  if (lefttasterstatus == HIGH) {
    if ((current_millis - last_wait_button_millis) > wait_button_time) {
      Serial.println("lefthigh");
      last_wait_button_millis = current_millis;
      action = true;
      runs = 1;
    }
  }

  if (righttasterstatus == HIGH) {
    if ((current_millis - last_wait_button_millis) > wait_button_time) {
      Serial.println("righhigh");
      last_wait_button_millis = current_millis;
      action = true;
      runs = 2;
    }
  }
}

