#include "objects.h"
#include "functionality.h"

void initOutputs() {
#ifdef LIGHT_PORT
  pinMode(LIGHT_PORT, OUTPUT);
#endif

#ifdef PUMP_PORT
  pinMode(PUMP_PORT, OUTPUT);
#endif

  // привод
#ifdef DRIVER_LEVEL
  // привод
  // частота на пинах 3 и 11 - 31.4 кГц
  TCCR2A |= _BV(WGM20);
  TCCR2B = TCCR2B & 0b11111000 | 0x01;

  /*// частота на пинах 3 и 11 - 7.8 кГц
    TCCR2A |= _BV(WGM20) | _BV(WGM21);
    TCCR2B = TCCR2B & 0b11111000 | 0x02;*/
  pinMode(DRV_PWM, OUTPUT);
  pinMode(DRV_SIGNAL1, OUTPUT);
  pinMode(DRV_SIGNAL2, OUTPUT);
  digitalWrite(DRV_SIGNAL1, DRIVER_LEVEL);
  digitalWrite(DRV_SIGNAL2, DRIVER_LEVEL);
  analogWrite(DRV_PWM, settings.driveSpeed);
#endif

  servo1.attach(SERVO1, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  servo2.attach(SERVO2, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
}

void updateOutputs() {
  static uint64_t ventelationTimer = 0;
  bool flaps = false, doorOpen = false;

  if (uptime - ventelationTimer >= settings.ventelationPeriod * 60) ventelationTimer = uptime;
  if (raining && settings.closeIfRain) flaps = false, doorOpen = false;
  else {
    if (airT > settings.flapsTemperatureThreshold) flaps = true;
    if (airT > settings.doorTemperatureThreshold) doorOpen = true;
    else if (uptime - ventelationTimer < settings.ventelationTime * 60) {
      flaps = true;
      doorOpen = true;
    }
  }

  if (!flap1.manual) flap1.state = flaps * 180;
  if (!flap2.manual) flap2.state = flaps * 180;
  if (!door.manual) door.state = doorOpen;
  if (!pump.manual) {
    static uint64_t wateringTimer = 0, wateringPeriodTimer = 0;
    if (uptime - wateringPeriodTimer >= settings.wateringPeriod * 60) {
      wateringPeriodTimer = uptime;
      pump.state = settings.wateringTime;
    }
    if (uptime - wateringTimer >= settings.wateringTimeout) {
      wateringTimer = uptime;
      if (soilMoisture < settings.wateringMoistureThreshold) {
        pump.state = settings.wateringTime;
      }
    }
  }
  {
    static uint64_t pumpTimer = 0;
    if (uptime - pumpTimer >= 1 && pump.state > 0) {
      pumpTimer = uptime;
      pump.state--;
    }
  }
  if (!enoughWater) pump.state = 0;

  if (!light.manual) {
    light.state = lightLevel < settings.lightLevelThreshold;
  }

  servo1.write(flap1.state);
  servo2.write(flap2.state);

  #ifdef LIGHT_PORT
    digitalWrite(LIGHT_PORT, !light.state);
  #endif

  #ifdef PUMP_PORT
    digitalWrite(PUMP_PORT, pump.state == 0);
  #endif

#ifdef DRIVER_LEVEL
  digitalWrite(DRV_SIGNAL1, DRIVER_LEVEL != door.state);
  digitalWrite(DRV_SIGNAL2, DRIVER_LEVEL != !door.state);
  analogWrite(DRV_PWM, settings.driveSpeed);
#endif
  sendStates();
}