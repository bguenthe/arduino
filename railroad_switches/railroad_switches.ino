#include <Bounce2.h>
#include <Servo.h>

int LEFT_REDLED_PIN = 10;
int LEFT_GREENLED_PIN = 11;

int LEFT_BUTTON_PIN = 8;

int LEFT_SERVO = 7;

bool left_status = true;
int left_changed = true;

Bounce leftbouncer = Bounce();

int lefttasterstatus;
int oldlefttasterstatus;

Servo left_servo;

void setup() {
  pinMode(LEFT_REDLED_PIN, OUTPUT);
  pinMode(LEFT_GREENLED_PIN, OUTPUT);

  pinMode(LEFT_BUTTON_PIN, INPUT);

  Serial.begin(9600);

  leftbouncer.attach(LEFT_BUTTON_PIN);
  leftbouncer.interval(5);
 
  left_servo.attach(LEFT_SERVO);
  left_servo.write(0); // Angle 0°
}

void loop() {
  if (left_status == true) {
    left_servo.write(0); //Angle 0°
    digitalWrite(LEFT_GREENLED_PIN, HIGH);
    digitalWrite(LEFT_REDLED_PIN, LOW);
  } else {
    left_servo.write(90); //Angle 90°
    digitalWrite(LEFT_GREENLED_PIN, LOW);
    digitalWrite(LEFT_REDLED_PIN, HIGH);
  }

  leftbouncer.update();
  lefttasterstatus = leftbouncer.read();

  if (lefttasterstatus == HIGH) {
    if (oldlefttasterstatus != lefttasterstatus) {
      if (left_status == true) {
        left_status = false;
      } else {
        left_status = true;
      }
    }
  }

  oldlefttasterstatus = lefttasterstatus;
}

