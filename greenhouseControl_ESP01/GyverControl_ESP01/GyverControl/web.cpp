#include "objects.h"
#include "functionality.h"

static void sendHTTPResponce(uint16_t code, String payload = "") {
  Serial.print(code);
  Serial.print(';');
  Serial.println(payload);
}

template<typename T>
void sendTableEntry(String name, T value) {
  Serial.print(name);
  Serial.print(':');
  Serial.println(value);
}

void sendTableEntry(String name, String value) {
  Serial.print(name);
  Serial.print(":\"");
  Serial.print(value);
  Serial.println('"');
}

void sendTableEntry(String name, bool value) {
  Serial.print(name);
  Serial.print(':');
  Serial.println(value ? "true" : "false");
}

void sendSensorValues() {
  Serial.println(";sensors");
  sendTableEntry("airT", airT);
  sendTableEntry("airH", airH);
  sendTableEntry("insideT", insideT);
  sendTableEntry("lightLevel", lightLevel);
  sendTableEntry("soilMoisture", soilMoisture);
  sendTableEntry("enoughWater", enoughWater);
  sendTableEntry("raining", raining);
  Serial.println("end:");
}

void sendStates() {
  Serial.println(";states");
  sendTableEntry("flap1", flap1.state);
  sendTableEntry("flap2", flap2.state);
  sendTableEntry("door", door.state);
  sendTableEntry("light", light.state);
  Serial.println("end:");
}

static bool stringToBool(const String& str) {
  if (str == "true") return true;
  else if (str == "false") return false;
  else return str.toInt();
}

void updateServer() {
  while (Serial.available() && Serial.peek() != ';') Serial.read();  // Sync
  if (Serial.peek() == ';') {
    Serial.read();
    String message = Serial.readStringUntil('\n');
    message.trim();
    if (message == "manual") {
      String invalid = "";
      while (true) {
        String name = Serial.readStringUntil(':');
        name.trim();
        if (name == "end") break;
        String value = Serial.readStringUntil('\n');
        value.trim();

#define CONTROL(object, ...) \
  if (name == #object) { \
    if (value == "\"auto\"") object.manual = false; \
    else { \
      object.manual = true; \
      object.state = __VA_ARGS__; \
    } \
  }

        CONTROL(flap1, value.toInt())
        else CONTROL(flap2, value.toInt())
        else CONTROL(door, stringToBool(value))
        else CONTROL(light, stringToBool(value))
        else invalid = name;
      }
      EEPROM.put(SETTINGS_ADDRESS, settings);
      if (invalid.length() > 0) sendHTTPResponce(422, "Unknown field: " + invalid);
      else sendHTTPResponce(200);
    } else if (message == "getManual") {
      sendTableEntry("flap1", flap1.manual);
      sendTableEntry("flap2", flap2.manual);
      sendTableEntry("door", door.manual);
      sendTableEntry("light", light.manual);
      Serial.println("end:");
    } else if (message == "settings") {
      String invalid = "";
      while (true) {
        String name = Serial.readStringUntil(':');
        name.trim();
        if (name == "end") break;
        String value = Serial.readStringUntil('\n');
        value.trim();

        if (name == "driveSpeed") settings.driveSpeed = value.toInt();
        else if (name == "sensorPeriod") settings.sensorPeriod = value.toInt();

        else if (name == "flapsTemperatureThreshold") settings.flapsTemperatureThreshold = value.toFloat();
        else if (name == "doorTemperatureThreshold") settings.doorTemperatureThreshold = value.toFloat();
        else if (name == "ventelationPeriod") settings.ventelationPeriod = value.toInt();
        else if (name == "ventelationTime") settings.ventelationTime = value.toInt();
        else if (name == "closeIfRain") settings.closeIfRain = stringToBool(value);

        else if (name == "wateringMoistureThreshold") settings.wateringMoistureThreshold = value.toInt();
        else if (name == "wateringTime") settings.wateringTime = value.toInt();
        else if (name == "wateringTimeout") settings.wateringTimeout = value.toInt();
        else if (name == "wateringPeriod") settings.wateringPeriod = value.toInt();

        else if (name == "lightLevelThreshold") settings.lightLevelThreshold = value.toInt();
        else invalid = name;
      }
      EEPROM.put(SETTINGS_ADDRESS, settings);
      if (invalid.length() > 0) sendHTTPResponce(422, "Unknown field: " + invalid);
      else sendHTTPResponce(200);
    } else if (message == "getSettings") {
      sendTableEntry("driveSpeed", settings.driveSpeed);
      sendTableEntry("sensorPeriod", settings.sensorPeriod);

      sendTableEntry("flapsTemperatureThreshold", settings.flapsTemperatureThreshold);
      sendTableEntry("doorTemperatureThreshold", settings.doorTemperatureThreshold);
      sendTableEntry("ventelationPeriod", settings.ventelationPeriod);
      sendTableEntry("ventelationTime", settings.ventelationTime);
      sendTableEntry("closeIfRain", settings.closeIfRain);

      sendTableEntry("wateringMoistureThreshold", settings.wateringMoistureThreshold);
      sendTableEntry("wateringTime", settings.wateringTime);
      sendTableEntry("wateringTimeout", settings.wateringTimeout);
      sendTableEntry("wateringPeriod", settings.wateringPeriod);

      sendTableEntry("lightLevelThreshold", settings.lightLevelThreshold);
      Serial.println("end:");
    } else if (message == "factoryReset") {
      settings = Settings();
      EEPROM.put(SETTINGS_ADDRESS, settings);
      sendHTTPResponce(200);
    } else {
      sendHTTPResponce(500, "Unknown operation: " + message);
    }
  }
}
