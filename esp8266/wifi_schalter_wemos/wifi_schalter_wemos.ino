/*
 * Empfängt mqtt-messages zum schalten eines relais via mqtt. 
 * sendet den schalter-status via mqtt
 * sendet die aktuelle temperatur via mqtt
 */

#include <PubSubClient_oo.h>

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

// Update these with values suitable for your network.
const char* ssid = "claube";
const char* password = "Nismipf01!";
const char* mqtt_server = "192.168.178.35";
const char* devicename = "wemos02";
String relay_status = "off";

int RELAY_PIN = D3;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

long last_millis = 0;

void publish(String par_topic, String status = "" ) {
  String from = "/from/";
  String topic = from + par_topic;
  StaticJsonBuffer<1024> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["device"] = devicename;
  root["time"] = 0; // for later use
  if (status.length() != 0) {
    root["relay-status"] = status;
  }

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
  //publish("signon");
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

  String todevice = json_root["device"];
  if (todevice.length() == 0 || todevice == devicename || todevice == "all") { // nur, wenn message für dieses geraet bestimt ist
    Serial.print("Device: ");
    Serial.println(todevice);
    // relay-switch
    if (topic == "/to/switch") {
      String switchvalue = json_root["value"];
      Serial.println(switchvalue);
      if (switchvalue == "on") {
        pinMode(RELAY_PIN, OUTPUT);
        digitalWrite(RELAY_PIN, 0);
        digitalWrite(BUILTIN_LED, 1);
        relay_status = "on";
        publish("switch", relay_status);
      } else {
        pinMode(RELAY_PIN, INPUT);
        digitalWrite(BUILTIN_LED, 0);
        relay_status = "off";
        publish("switch", relay_status);
      }
    } else if (topic == "/to/status") {
      publish("switch", relay_status);
    }
  }
}

void setup() {
  Serial.begin(115200);
 
  mqttClient.set_server(mqtt_server, 1883);
  mqttClient.set_callback(callback);

  //pinMode(RELAY_PIN, INPUT);
  pinMode(BUILTIN_LED, OUTPUT); // initialize onboard LED as output
  relay_status = "off";
  publish("switch", relay_status);
  wifi_start();
  mqtt_start();

  publish("signon");
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
}
