#pragma once
#include "settings.h"

#include <EEPROM.h>

#include <LiquidCrystal_I2C.h>
extern LiquidCrystal_I2C lcd;

#include <EncButton.h>
extern EncButton enc;

#ifdef BME_ADDR
#include <GyverBME280.h>
extern GyverBME280 bme;
#endif

#if defined(DHT_PORT) && defined(DHT_TYPE)
#include <DHT.h>
extern DHT dht;
#endif

#ifdef LIGHT_SENSOR_ENABLED
#include <BH1750.h>
extern BH1750 lightSensor;
#endif

#include <DS1307RTC.h>

#include <Servo.h>
extern Servo servo1;
extern Servo servo2;

#if defined(CRITICAL_HIGH_TEMPERATURE) || defined(CRITICAL_LOW_TEMPERATURE)
extern bool criticalTemperature;
#endif

const uint32_t MAGIC = 0xABCD1234;
const uint32_t SETTINGS_ADDRESS = 0;
struct Settings {
  uint32_t magic = MAGIC;
  uint8_t driveSpeed = 125;
  uint16_t sensorPeriod = 1;  

  float flapsTemperatureThreshold = 30.0;  
  float doorTemperatureThreshold = 30.0;   
  uint16_t ventelationPeriod = 60;         
  uint16_t ventelationTime = 10;          
  bool closeIfRain = true;                

  uint8_t wateringMoistureThreshold = 50; 
  uint16_t wateringTime = 10;             
  uint16_t wateringTimeout = 30;          
  uint16_t wateringPeriod = 30;           

  uint8_t lightLevelThreshold = 50;       
};
extern Settings settings;

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
