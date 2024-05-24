#pragma once
#include <Arduino.h>

// Encoder
#define SW 0
#define DT 2
#define CLK 3

// Relays
#define RELAY1 1
#define RELAY2 4
#define RELAY3 5
#define RELAY4 6
#define RELAY5 7
#define RELAY6 8
#define RELAY7 9
const uint8_t relayPins[] = {RELAY1, RELAY2, RELAY3, RELAY4, RELAY5, RELAY6, RELAY7};

// Servo
#define SERVO1 13
#define SERVO2 A0

// Driver
#define DRV_SIGNAL1 10
#define DRV_SIGNAL2 12
#define DRV_PWM 11

// Sensors
#define SENS_VCC A1
#define SENS1 A2
#define SENS2 A3
#define SENS3 A6
#define SENS4 A7
const uint8_t sensorPins[] = {SENS1, SENS2, SENS3, SENS4};
