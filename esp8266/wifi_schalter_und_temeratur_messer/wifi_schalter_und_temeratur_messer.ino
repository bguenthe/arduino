/*
 * Empfängt mqtt-messages zum schalten eines relais via mqtt. 
 * sendet den schalter-status via mqtt
 * sendet die aktuelle temperatur via mqtt
 */
#include <PubSubClient.h>

#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Update these with values suitable for your network.
const char* ssid = "claube";
const char* password = "Nismipf01!";
const char* mqtt_server = "192.168.178.35";
const char* devicename = "wifidevice_huzzah";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

long last_millis = 0;
char msg[50];

int RELAY_PIN = 12;

// DHT22
// Version number
const float fVerNum = 0.03;

// Data pin connected to AM2302
#define DHTPIN 5
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE, 15);

void publish(String par_topic, String msg = "") {
  String from = "/from/";
  String topic = from + topic;
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["device"] = devicename;
  root["time"] = 0; // for later use
  if (msg.length() != 0) { // hier noch den uebergebenen string parsen und json draus machen (wenn möglich)
    root["msg"] = msg;
  }

  char buffer[256];
  root.printTo(buffer, sizeof(buffer));

  Serial.println(buffer);
  mqttClient.publish(topic.c_str(), buffer);
}

void wifi_start() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.hostname(devicename);
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

void mqtt_start() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(devicename)) {
      Serial.println("connected");
      // subscribe
      mqttClient.subscribe("/to/#"); // alle to meldungen
    } else {
      Serial.print("failed");
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  publish("signon");
}

void callback(char* par_topic, byte* par_payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(par_topic);
  String topic = par_topic;
  Serial.print("] ");

  char payload[length + 1];
  for (int i=0;i<length;i++) {
    payload[i] = (char)par_payload[i];
  }

  Serial.print(payload);
  Serial.println();


  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(payload);
  char buffer[256];
  root.printTo(buffer, sizeof(buffer));
  Serial.println(buffer);

  // relay-switch
  if (topic == "/to/switch") {
    const char* switchvalue = root["value"];
    if (switchvalue == "on") {
      pinMode(RELAY_PIN, OUTPUT);
      digitalWrite(RELAY_PIN, 0);
    } else {
      pinMode(RELAY_PIN, INPUT);
    }
  }
}

void setup() {
  Serial.begin(115200);
   
  wifi_start();
  
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(callback);

  mqtt_start();

  pinMode(RELAY_PIN, INPUT);
  relay_status = "off";
  publish("switch", "", "", relay_status);

  dht.begin();
}

void loop() {
  // check if WLAN is connected and reconnect if not
  if (WiFi.status() != WL_CONNECTED) {
    wifi_start();
  }

  // check if mqttbroker is connected and reconnect if not
  if (!mqttClient.connected()) {
    mqtt_start();
  }
  mqttClient.loop();

  long current_millis = millis();
  if (current_millis - last_millis > 60000) { // Alle 15 Sekunden
    last_millis = current_millis;
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
    publish("temperature", msg);

    dtostrf(h, 4, 2, msg);
    Serial.print("Publish humidity: ");
    Serial.println(msg);
    publish("humidity", msg);
  }
}
