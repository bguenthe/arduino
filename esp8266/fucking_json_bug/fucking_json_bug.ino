/*
 * Empfängt mqtt-messages zum schalten eines relais via mqtt. 
 * sendet den schalter-status via mqtt
 * sendet die aktuelle temperatur via mqtt
 */

#include <ArduinoJson.h>

// Update these with values suitable for your network.
String relay_status = "off";

void publish(String par_topic, String humidity = "", String temperature = "", String status = "" ) {
  String from = "/from/";
  String topic = from + par_topic;
  StaticJsonBuffer<1024> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["device"] = "ESB";
  root["time"] = 0; // for later use
  if (humidity.length() != 0) {
    root["value"] = humidity;
  } else if (temperature.length() != 0) {
    root["value"] = humidity;
  } else if (status.length() != 0) {
    root["value"] = status;
  }

  String payload;
  root.printTo(payload);

  Serial.println(topic);
  Serial.println(payload);
}

void callback(String par_topic, String par_payload) {
  Serial.print("Message arrived [");
  Serial.print(par_topic);
  String topic = par_topic;
  Serial.print("] ");

  Serial.print(par_payload);
  Serial.println();

  String payload = par_payload;

  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& json_root = jsonBuffer.parseObject(payload);
  char buffer[1024];
  json_root.printTo(buffer, sizeof(buffer));
  Serial.println(buffer);

  publish("/from/test", relay_status);
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  callback("zesz", "test");
  delay(5000);
}
