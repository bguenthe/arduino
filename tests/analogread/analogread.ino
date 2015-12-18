int analogValue;

void setup() {
  Serial.begin(115200);
  pinMode(0, INPUT);
}

void loop() {
  analogValue = analogRead(0);
  Serial.println(analogValue);
  delay(1000);
}
