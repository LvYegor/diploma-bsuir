#pragma once
#include <Arduino.h>

// * Sensors
enum class SensorReadingStage {
  PowerOn,
  Read,
  PowerOff,
  UpdateOutputs,
};

extern SensorReadingStage sensorReadingStage;

void initSensors();
void readTemperatures();
void readAllSensors();
void updateTime();

// * Outputs
void initOutputs();
void updateOutputs();

// * Web
void sendSensorValues();
void sendStates();
void updateServer();

// * LCD
enum {
  LCD_WATER = 0,
  LCD_NO_WATER,
  LCD_LIGHT,
  LCD_CLOUD,
  LCD_FLOWER,
  LCD_PERCENT = 37,
  LCD_DEGREE = 223,
};

void initLCD();
void updateLCD();
