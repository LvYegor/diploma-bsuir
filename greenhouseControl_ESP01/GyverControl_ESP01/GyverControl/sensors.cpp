#include "objects.h"
#include "functionality.h"

SensorReadingStage sensorReadingStage = SensorReadingStage::PowerOn;

static int charToDec(const char* p) {
  return ((*p - '0') * 10 + (*(p + 1) - '0'));
}

void initSensors() {
  pinMode(SENS_VCC, OUTPUT);

  // RTC
  if (!RTC.read(datetime) || (datetime.Year == 2000 && datetime.Month == 1 && datetime.Day == 1)) {
    char* stamp = __TIMESTAMP__;
    datetime.Hour = charToDec(stamp + 11);
    datetime.Minute = charToDec(stamp + 14);
    datetime.Second = charToDec(stamp + 17);
    datetime.Day = charToDec(stamp + 8);
    switch (stamp[4]) {
      case 'J': datetime.Month = (stamp[5] == 'a') ? 1 : ((stamp[6] == 'n') ? 6 : 7); break;
      case 'F': datetime.Month = 2; break;
      case 'A': datetime.Month = (stamp[6] == 'r') ? 4 : 8; break;
      case 'M': datetime.Month = (stamp[6] == 'r') ? 3 : 5; break;
      case 'S': datetime.Month = 9; break;
      case 'O': datetime.Month = 10; break;
      case 'N': datetime.Month = 11; break;
      case 'D': datetime.Month = 12; break;
    }
    datetime.Year = 2000 + charToDec(stamp + 22);
    RTC.write(datetime);
  }
  // if (rtc.lostPower()) {        //  при потере питания
  //   rtc.setTime(COMPILE_TIME);  // установить время компиляции
  // }

  // BME
#ifdef BME_ADDR
  bme.begin(BME_ADDR);
#endif

  // DHT
#if defined(DHT_PORT) && defined(DHT_TYPE)
  dht.begin();
#endif

// Light
#ifdef LIGHT_SENSOR_ENABLED
  lightSensor.begin();
#endif

// Rain
#ifdef RAIN_PORT
  pinMode(RAIN_PORT, INPUT);
#endif

  uptime = 0;
}

float analogReadAverage(uint8_t pin) {
  // пропускаем первые 10 измерений
  for (uint8_t i = 0; i < 10; i++) analogRead(pin);

  // суммируем
  uint16_t sum = 0;
  for (uint8_t i = 0; i < 10; i++) sum += analogRead(pin);
  return sum / 10.0;
}

void readTemperatures() {
  float dhtT = NAN, dhtH = NAN;
  float bmeT = NAN, bmeH = NAN;

#if defined(DHT_PORT) && defined(DHT_TYPE)
  dhtT = dht.readTemperature();
  dhtH = dht.readHumidity();
#endif

#ifdef BME_ADDR
  bmeT = bme.readTemperature();
  bmeH = bme.readHumidity();
#endif

  if (!isnan(dhtT) && !isnan(bmeT)) airT = dhtT, insideT = bmeT;
  else if (!isnan(dhtT)) airT = insideT = dhtT;
  else if (!isnan(bmeT)) airT = insideT = bmeT;

  if (!isnan(dhtH) && !isnan(bmeH)) airH = dhtH, insideH = bmeH;
  else if (!isnan(dhtH)) airH = insideH = dhtH;
  else if (!isnan(bmeH)) airH = insideH = bmeH;

  insideH = constrain(insideH, 0, 99);
  airH = constrain(airH, 0, 99);
}

static void getAllData() {
  readTemperatures();

#ifdef LIGHT_SENSOR_ENABLED
  if (lightSensor.measurementReady()) lightLevel = lightSensor.readLightLevel();
#endif

#ifdef SOIL_MOISTURE_PORT
  {
    float moisture = 100.0 - (analogReadAverage(SOIL_MOISTURE_PORT) - 400.0) / 5.7;
    soilMoisture = constrain(moisture, 0.0, 100.0);
  }
#endif

#ifdef WATER_LEVEL_PORT
  enoughWater = analogRead(WATER_LEVEL_PORT) > 256;
#endif

#ifdef RAIN_PORT
  raining = !digitalRead(RAIN_PORT);
#endif

  sendSensorValues();
}

void readAllSensors() {
  static uint64_t timer = 0;
  static uint32_t period = 1000;

  if (millis() - timer >= period) {
    timer = millis();
    switch (sensorReadingStage) {
      case SensorReadingStage::PowerOn:
        sensorReadingStage = SensorReadingStage::Read;
        period = 100;
        digitalWrite(SENS_VCC, 1);
        break;
      case SensorReadingStage::Read:
        sensorReadingStage = SensorReadingStage::PowerOff;
        period = 25;
        getAllData();
        break;
      case SensorReadingStage::PowerOff:
        sensorReadingStage = SensorReadingStage::UpdateOutputs;
        period = 10;
        digitalWrite(SENS_VCC, 0);
        break;
      case SensorReadingStage::UpdateOutputs:
        sensorReadingStage = SensorReadingStage::PowerOn;
        period = (long)settings.sensorPeriod * 1000;
        updateOutputs();
        break;
    }
  }
}

void updateTime() {
  static uint64_t timer = 0;

  if (millis() - timer > 1000) {
    timer += 1000;
    RTC.read(datetime);
    uptime++;
  }
}
