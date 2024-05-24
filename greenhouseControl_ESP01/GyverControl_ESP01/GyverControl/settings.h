#pragma once
#include "pins.h"

// * Settings
// * Comment out to disable
// Note: REL1 must not be used, since it's used by UART

// DHT
#define DHT_PORT SENS1  // Must be SENS1/SENS2, since other SENS ports can't be used as digital
#define DHT_TYPE DHT22

// BME280 (шина i2c) - цифровой датчик температуры и влажности
// Falls back to it, if DHT fails
#define BME_ADDR 0x76  // адрес BME280 - 0x76 или 0x77. Смени если не работает!

// Sensors & Outputs
#define LIGHT_SENSOR_ENABLED
#define SOIL_MOISTURE_PORT SENS4
#define WATER_LEVEL_PORT SENS3
#define RAIN_PORT SENS2  // Must be SENS1/SENS2, since other SENS ports can't be used as digital
#define LIGHT_PORT RELAY3
#define PUMP_PORT RELAY4

// Servo
#define SERVO_MIN_PULSE 500   // минимальный импульс серво (зависит от модели, 500-800)
#define SERVO_MAX_PULSE 2500  // максимальный импульс серво (зависит от модели, 2000-2500)

// Misc
#define DRIVER_LEVEL 1                // 1 или 0 - уровень сигнала на драйвер/реле для привода
#define LCD_BACKLIGHT_TIMEOUT 60      // таймаут отключения дисплея, секунды
#define CRITICAL_HIGH_TEMPERATURE 50  // If inside temperature (deg C) is higher than this number, controller will turn off
#define CRITICAL_LOW_TEMPERATURE -25  // If inside temperature (deg C) is lower than this number, controller will turn off

#define WIRE_OVERCLOCK 400000  // увеличить частоту шины i2c до 400 кГц
#define LCD_ADDR 0x27          // адрес дисплея - 0x27 или 0x3f . Смени если не работает!
