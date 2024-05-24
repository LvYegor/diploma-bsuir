#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>

const char* ssid = "TP-Link_49F2";
const char* password = "06802912";

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);

String sensors = "";
String states = "";

void getSensors() {
  if (sensors.isEmpty()) server.send(503, "text/plain", "Not yet loaded");
  else server.send(200, "text/plain", sensors);
}

void getStates() {
  if (states.isEmpty()) server.send(503, "text/plain", "Not yet loaded");
  else server.send(200, "application/json", states);
}

static String readJsonTable() {
  String json = "{";
  while (true) {
    String name = Serial.readStringUntil(':');
    name.trim();
    if (name == "end") break;
    String value = Serial.readStringUntil('\n');
    value.trim();
    json += '"' + name + "\":" + value + ",";
  }
  if (json.endsWith(",")) json.remove(json.length() - 1, 1);
  return json + "}";
}

static void checkSerial() {
  while (Serial.available() && Serial.peek() != ';') Serial.read();  // Sync
  if (Serial.peek() == ';') {
    Serial.read();
    String message = Serial.readStringUntil('\n');
    message.trim();
    if (message == "sensors") sensors = readJsonTable();
    else if (message == "states") states = readJsonTable();
    else if (message == "wifi on") {
      WiFi.mode(WIFI_STA);
      while (wifiMulti.run() != WL_CONNECTED) {
        delay(500);
      }
    } else if (message == "wifi off") WiFi.mode(WIFI_OFF);
  }
}

static void getResponce() {
  String codeText = Serial.readStringUntil(';');
  codeText.trim();
  uint16_t code = codeText.toInt();
  String payload = Serial.readStringUntil('\n');
  payload.trim();
  if (payload.isEmpty()) server.send(code);
  else server.send(code, "text/plain", payload);
}

void setManual() {
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  Serial.println(";manual");
  for (JsonPair kv : doc.as<JsonObject>()) {
    Serial.print(kv.key().c_str());
    Serial.print(':');
    serializeJson(kv.value(), Serial);
    Serial.println();
  }
  Serial.println("end:");
  getResponce();
}

void getManual() {
  Serial.println(";getManual");
  server.send(200, "application/json", readJsonTable());
}

void setSettings() {
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  Serial.println(";settings");
  for (JsonPair kv : doc.as<JsonObject>()) {
    Serial.print(kv.key().c_str());
    Serial.print(':');
    serializeJson(kv.value(), Serial);
    Serial.println();
  }
  Serial.println("end:");
  getResponce();
}

void getSettings() {
  Serial.println(";getSettings");
  server.send(200, "application/json", readJsonTable());
}

void factoryReset() {
  Serial.println(";factoryReset");
  getResponce();
}

void setup() {
  Serial.begin(9600);
  wifiMulti.addAP(ssid, password);
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
  }
  server.on("/sensors", HTTP_GET, getSensors);
  server.on("/states", HTTP_GET, getStates);
  server.on("/manual", HTTP_POST, setManual);
  server.on("/manual", HTTP_GET, getManual);
  server.on("/settings", HTTP_POST, setSettings);
  server.on("/settings", HTTP_GET, getSettings);
  server.on("/factoryReset", HTTP_POST, factoryReset);
  server.begin();  // Starting the web server
}

void loop() {
  checkSerial();
  server.handleClient();
}
