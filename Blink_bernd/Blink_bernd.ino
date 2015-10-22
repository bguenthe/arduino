/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
  Läuft auf meinem arduino nano atmega328
 */
const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
const long interval = 1000;  // interval at which to blink (milliseconds)
 
float counter = 0;
int buttonState = 0;         // variable for reading the pushbutton status
int previousButtonState = 0;         // variable for reading the pushbutton status
int ledState = LOW;             // ledState used to set the LED

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated


// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(buttonPin, INPUT);
  
  // LED blinken
  pinMode(ledPin, OUTPUT); // interne LED
  Serial.begin(9600);           // set up Serial library at 9600 bps
}

// the loop function runs over and over again forever
void loop() {
  // buttom stuff
  // read the state of the switch into a local variable:1
  int reading = digitalRead(buttonPin);
  if (reading != previousButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
        Serial.println("{\"buttom\":\"TRUE\"}");
      } else {
        Serial.println("{\"buttom\":\"FALSE\"}");
      }
    }
  }
  
  digitalWrite(ledPin, ledState);
  
  // LED-Stuff
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
      counter++;
      String scounter = "{\"counter\":\"";
      scounter += counter;
      scounter += "\"}";
      Serial.println(scounter);
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
  previousButtonState = reading;
}
