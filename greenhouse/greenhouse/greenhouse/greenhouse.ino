#include "objects.h"
#include "functionality.h"

void setup() {
  Serial.begin(9600);

  EEPROM.get(SETTINGS_ADDRESS, settings);
  if (settings.magic != MAGIC) {
    settings = Settings();
    EEPROM.put(SETTINGS_ADDRESS, settings);
  }

  #if defined(WIRE_OVERCLOCK)
    Wire.setClock(WIRE_OVERCLOCK);
  #endif

  EICRA = (EICRA & 0x0C) | 1; 
  bitSet(EIMSK, INT0);        
  EICRA = (EICRA & 0x03) | (1 << 2);
  bitSet(EIMSK, INT1);
  enc.setEncISR(true);

  initOutputs();
  initSensors();
  initLCD();
}

void loop() {
  #if defined(CRITICAL_HIGH_TEMPERATURE) || defined(CRITICAL_LOW_TEMPERATURE)
    #ifndef CRITICAL_HIGH_TEMPERATURE
      if (insideT <= CRITICAL_LOW_TEMPERATURE) {
    #elif !defined(CRITICAL_LOW_TEMPERATURE)
      if (insideT >= CRITICAL_HIGH_TEMPERATURE) {
    #else
      if (insideT <= CRITICAL_LOW_TEMPERATURE || insideT >= CRITICAL_HIGH_TEMPERATURE) {
    #endif

    if (!criticalTemperature) {
      criticalTemperature = true;

      digitalWrite(SENS_VCC, 0);
      sensorReadingStage = SensorReadingStage::PowerOn;

      #ifdef LIGHT_PORT
        digitalWrite(LIGHT_PORT, HIGH);
      #endif

      #ifdef PUMP_PORT
        digitalWrite(PUMP_PORT, HIGH);
      #endif

      #ifdef DRIVER_LEVEL
        digitalWrite(DRV_SIGNAL1, DRIVER_LEVEL);
        digitalWrite(DRV_SIGNAL2, DRIVER_LEVEL);
        digitalWrite(DRV_PWM, LOW);
      #endif

      Serial.println(";wifi off");

      lcd.backlight();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Critical Temp!");
    }

    readTemperatures();
    lcd.setCursor(0, 1);
    lcd.print("Temperature: ");
    if (insideT >= 0) lcd.print(' ');
    if (abs(insideT) < 10) lcd.print(' ');
    lcd.print(insideT, 1);
    lcd.write(LCD_DEGREE);
    return;
  } else if (criticalTemperature) {
    criticalTemperature = false;
    Serial.println(";wifi on");
  }
  #endif

  updateTime();
  readAllSensors();
  updateLCD();
  updateServer();
}

ISR(INT0_vect) {
  enc.tickISR();
}

ISR(INT1_vect) {
  enc.tickISR();
}
