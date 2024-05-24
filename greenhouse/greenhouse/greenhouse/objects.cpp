#include "objects.h"

LiquidCrystal_I2C lcd(LCD_ADDR, 20, 4);
EncButton enc(CLK, DT, SW);
Servo servo1;
Servo servo2;

#ifdef BME_ADDR
GyverBME280 bme;
#endif

#if defined(DHT_PORT) && defined(DHT_TYPE)
DHT dht(DHT_PORT, DHT_TYPE);
#endif

#ifdef LIGHT_SENSOR_ENABLED
#include <BH1750.h>
BH1750 lightSensor;
#endif

#if defined(CRITICAL_HIGH_TEMPERATURE) || defined(CRITICAL_LOW_TEMPERATURE)
bool criticalTemperature = false;
#endif

Settings settings;

tmElements_t datetime;
uint64_t uptime = 0;
float airT = 25.0, airH = 30.0;
float insideT = airT, insideH = airH;
uint16_t lightLevel = 0;
uint8_t soilMoisture = 100;
bool enoughWater = true;
bool raining = false;

Output flap1;
Output flap2;
Output door;
Output pump;
Output light;