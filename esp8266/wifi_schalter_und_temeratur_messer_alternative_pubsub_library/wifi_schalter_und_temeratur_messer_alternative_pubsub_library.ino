/*
 * Empfängt mqtt-messages zum schalten eines relais via mqtt. 
 * sendet den schalter-status via mqtt
 * sendet die aktuelle temperatur via mqtt
 */

#include <PubSubClient_oo.h>

#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Update these with values suitable for your network.
const char* ssid = "claube";
const char* password = "Nismipf01!";
const char* mqtt_server = "192.168.178.35";
const char* devicename = "wifidevice01";
String relay_status = "off";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

long last_millis = 0;
char msg[50];

// DHT22
// Version number
const float fVerNum = 0.03;

// Data pin connected to AM2302
#define DHTPIN 5
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE, 15);

void publish(String par_topic, String humidity = "", String temperature = "", String status1 = "" ) {
  String from = "/from/";
  String topic = from + par_topic;
  StaticJsonBuffer<1024> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["device"] = devicename;
  root["time"] = 0; // for later use
  if (humidity.length() != 0) {
    root["value"] = humidity;
  } else if (temperature.length() != 0) {
    root["value"] = humidity;
  } else if (status1.length() != 0) {
    root["value"] = status1;
  }

  int len = root.measureLength();
  Serial.println(len);
//  char payload[1024];
  String payload;
  root.printTo(payload);

  Serial.println(topic);
  Serial.println(payload);
  mqttClient.publish(topic, payload);
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
      mqttClient.subscribe(MQTT::Subscribe().add_topic("/to/#")); // alle to meldungen
    } else {
      Serial.print("failed");
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  publish("signon");
}

void callback(const MQTT::Publish& pub) {
  Serial.print("Message arrived [");
  Serial.print(pub.topic());
  String topic = pub.topic();
  Serial.print("] ");

  Serial.print(pub.payload_string());
  Serial.println();

  String payload = pub.payload_string();

  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& json_root = jsonBuffer.parseObject(payload);
  char buffer[1024];
  json_root.printTo(buffer, sizeof(buffer));
  Serial.println(buffer);

  // relay-switch
  if (topic == "/to/switch") {
    String switchvalue = json_root["value"];
    Serial.println(switchvalue);
    if (switchvalue == "on") {
      pinMode(12, OUTPUT);
      digitalWrite(12, 0);
      relay_status = "on";
      //publish("switch", "", "", "on");
      mqttClient.publish("/from/test", relay_status);
    } else {
      pinMode(12, INPUT);
      relay_status = "off";
      //publish("switch", "", "", "off");
      mqttClient.publish("/from/test", relay_status);
    }
  } else if (topic == "/to/status") {
    //publish("switch", "", "", relay_status);
  }
}

void setup() {
  Serial.begin(115200);

  wifi_start();
  
  mqttClient.set_server(mqtt_server, 1883);
  mqttClient.set_callback(callback);

  mqtt_start();

  pinMode(12, INPUT);
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
  if (current_millis - last_millis > 15000) { // Alle 15 Sekunden
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
    publish("temperature", "", msg);

    dtostrf(h, 4, 2, msg);
    Serial.print("Publish humidity: ");
    Serial.println(msg);
    publish("humidity", msg);
  }
}
