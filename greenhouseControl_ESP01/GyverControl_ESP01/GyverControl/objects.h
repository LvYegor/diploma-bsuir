#pragma once
#include "settings.h"

#include <EEPROM.h>

// LCD
#include <LiquidCrystal_I2C.h>
extern LiquidCrystal_I2C lcd;

// Encoder
#include <EncButton.h>
extern EncButton enc;

// BME
#ifdef BME_ADDR
#include <GyverBME280.h>
extern GyverBME280 bme;
#endif

// DHT
#if defined(DHT_PORT) && defined(DHT_TYPE)
#include <DHT.h>
extern DHT dht;
#endif

// GY-30 (Light Sensor)
#ifdef LIGHT_SENSOR_ENABLED
#include <BH1750.h>
extern BH1750 lightSensor;
#endif

// RTC
#include <DS1307RTC.h>

// Servo
#include <Servo.h>
extern Servo servo1;
extern Servo servo2;

#if defined(CRITICAL_HIGH_TEMPERATURE) || defined(CRITICAL_LOW_TEMPERATURE)
extern bool criticalTemperature;
#endif

// * EEPROM settings
const uint32_t MAGIC = 0xABCD1234;
const uint32_t SETTINGS_ADDRESS = 0;
struct Settings {
  uint32_t magic = MAGIC;
  uint8_t driveSpeed = 125;   // скорость привода, 0-255
  uint16_t sensorPeriod = 1;  // Read sensors every this much seconds

  float flapsTemperatureThreshold = 30.0;  // Open flaps if air temperature (deg C) is higher than the threshold
  float doorTemperatureThreshold = 30.0;   // Open door if air temperature (deg C) is higher than the threshold
  uint16_t ventelationPeriod = 60;         // Open door and flaps every this much minutes, regardless of temperature
  uint16_t ventelationTime = 10;           // Open door and flaps for ventelation for this much minutes
  bool closeIfRain = true;                 // Close door and flaps if raining

  uint8_t wateringMoistureThreshold = 50;  // Water if the soil moisture is lower than the threshold
  uint16_t wateringTime = 10;              // Water for this much seconds
  uint16_t wateringTimeout = 30;           // Don't water for this much seconds after we watered for wateringTime and the soil is still dry
  uint16_t wateringPeriod = 30;            // Water every this much minutes, regardless of soil moisture

  uint8_t lightLevelThreshold = 50;  // Turn on light if it's darker than the threshold
};
extern Settings settings;

// * Values
extern tmElements_t datetime;
extern uint64_t uptime;
extern float airT, airH;
extern float insideT, insideH;
extern uint16_t lightLevel;
extern uint8_t soilMoisture;
extern bool enoughWater;
extern bool raining;

struct Output {
  uint8_t state = 0;
  bool manual = false;
};

extern Output flap1;
extern Output flap2;
extern Output door;
extern Output pump;
extern Output light;
