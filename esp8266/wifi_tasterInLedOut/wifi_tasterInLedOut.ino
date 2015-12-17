#include <MQTT.h>
#include <PubSubClient.h>

#include <ESP8266WiFi.h>
#include <DHT.h>          // DHT & AM2302 library

// Update these with values suitable for your network.

const char* ssid = "claube";
const char* password = "Nismipf01!";
const char* mqtt_server = "192.168.178.33";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
long lastMsg = 0;
char msg[50];
int value = 0;

int tasterPin = 8;

int ledPins[] = {9, 10, 11, 12, 13};
boolean ledPinsStatus[] = {false, false, false, false, false};

int tasterStatus;
int lastStatus = LOW;
const String device = "from_arduino"; // used for the serial messages

// DHT22
// Version number
const float fVerNum = 0.03;

// Data pin connected to AM2302
#define DHTPIN 5
#define DHTTYPE DHT22       // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE, 15);

void wifi_start() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(const MQTT::Publish& pub) {
  Serial.print("Message arrived [");
  Serial.print(pub.topic());
  Serial.print("] ");

  Serial.print(pub.payload_string());
  Serial.println();

  String str = pub.payload_string();

  // relay-switch
  if (str.startsWith("/relay")) { // Beispiel: "/relay/on"
    if (str.endsWith("on")) {
      pinMode(12, OUTPUT);
      digitalWrite(12, 0);
    } else {
      pinMode(12, INPUT);
    }
    Serial.print("relay: ");
  }

  // LED-switch
  int mode = 0;
  if (str.startsWith("/LED")) { // Beispiel: "/LED1/on"
    if (str.endsWith("on")) {
      mode = LOW;
    } else {
      mode = HIGH;
    }
    digitalWrite(BUILTIN_LED, mode);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BUILTIN_LED, 1); // ausschalten mit 1!

  Serial.begin(115200);
  wifi_start();
  mqttClient.set_server(mqtt_server, 1883);
  mqttClient.set_callback(callback);

  dht.begin();
}


void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("ESP8266Client")) {
      Serial.println("connected");

      // subscribe
      mqttClient.subscribe(MQTT::Subscribe().add_topic("/arduino/LEDswitch").add_topic("/arduino/switch"));
    } else {
      Serial.print("failed");
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  // check if WLAN is connected and reconnect if not
  if (WiFi.status() != WL_CONNECTED) {
    wifi_start();
  }

  // check if mqttbroker is connected and reconnect if not
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  if (tasterStatus == LOW) {
    if (tasterStatus != lastStatus) {
      digitalWrite(ledPins[0], LOW);
      Serial.println("{\"/arduino/button_status\":\"FALSE\"}");
    }
  } else {
    if (tasterStatus != lastStatus) {
      digitalWrite(ledPins[0], HIGH);
      Serial.println("{\"/arduino/button_status\":\"TRUE\"}");
    }
  }

  long now = millis();
  if (now - lastMsg > 15000) { // Alle 15 Sekunden
    lastMsg = now;
    // -50 Grad = 0 V; 150 Grad = 2 V  Formel Grad =200*(Volt/2)-50
    // 0.7 V = 20 Grad
    // 
/*    int temperature = analogRead(A0);
    Serial.println(temperature);
    float volt = 1.192 / 1024 * temperature;
    Serial.println(volt);
    float grad = 200 * (volt / 2) - 50;
    Serial.println(grad);

    dtostrf(grad, 4, 2, msg);
    Serial.print("Publish message: ");
    Serial.println(msg);
    mqttClient.publish("/arduino/temperature", msg);
*/
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(t) || isnan(h)) {
      Serial.println(F("Failed to read from DHT"));
    } 
    dtostrf(t, 4, 2, msg);
    Serial.print("Publish temparatur: ");
    Serial.println(msg);
    mqttClient.publish("/arduino/temperature", msg);

    dtostrf(h, 4, 2, msg);
    Serial.print("Publish humidity: ");
    Serial.println(msg);
    mqttClient.publish("/arduino/humidity", msg);
  }
}
