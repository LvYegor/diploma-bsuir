#include "objects.h"
#include "functionality.h"

static uint8_t waterIcon[] = {
  B00100,
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110
};

static uint8_t noWaterIcon[] = {
  B00100,
  B00100,
  B01010,
  B01010,
  B10001,
  B10001,
  B10001,
  B01110
};

static uint8_t lgithIcon[] = {
  B00000,
  B10101,
  B01110,
  B11111,
  B01110,
  B10101,
  B00000,
  B00000
};

static uint8_t cloudIcon[] = {
  B01110,
  B11111,
  B11111,
  B00000,
  B01000,
  B00010,
  B01000,
  B00000
};

static uint8_t flowerIcon[] = {
  B00100,
  B01110,
  B00100,
  B00100,
  B11111,
  B11111,
  B01110,
  B01110
};

void initLCD() {
  lcd.init();
  lcd.createChar(LCD_WATER, waterIcon);
  lcd.createChar(LCD_NO_WATER, noWaterIcon);
  lcd.createChar(LCD_LIGHT, lgithIcon);
  lcd.createChar(LCD_CLOUD, cloudIcon);
  lcd.createChar(LCD_FLOWER, flowerIcon);
  lcd.backlight();
  lcd.clear();
}

static void controllable(Output& output, String name, uint8_t stateEnabled, uint8_t y, bool highlighted) {
  lcd.setCursor(0, y);
  if (highlighted) {
    lcd.print('>');
    if (enc.press()) {
      if (!output.manual) {
        output.manual = true;
        output.state = 0;
      } else if (!output.state) output.state = stateEnabled;
      else output.manual = false;
    }
  } else lcd.print(' ');
  lcd.print(name + ": ");
}

void updateLCD() {
  static uint64_t updateTimer = 0;
  static uint8_t page = 0, entry = 0;
  static bool factoryReset = false;
  enc.tick();
  if (enc.holding()) {
    door.state = 1;
    door.manual = true;
  }

  if (enc.turn()) {
    const uint8_t ENTRIES[] = { 1, 4, 1, factoryReset ? 2 : 1 };
    const uint8_t PAGES = sizeof(ENTRIES) / sizeof(ENTRIES[0]);
    int8_t newEntry = entry + enc.dir();
    if (factoryReset) {
      newEntry = (newEntry + ENTRIES[page]) % ENTRIES[page];
    } else if (newEntry < 0 || newEntry >= ENTRIES[page]) {
      lcd.clear();
      factoryReset = false;
      if (newEntry < 0) {
        page = (page + PAGES - 1) % PAGES;
        newEntry += ENTRIES[page];
      } else {
        newEntry %= ENTRIES[page];
        page = (page + 1) % PAGES;
      }
    }
    entry = newEntry;
  }

  if (millis() - updateTimer > settings.sensorPeriod * 1000 / 2 || enc.action()) {
    if (page == 0) {
      // Time
      lcd.setCursor(0, 0);
      if (datetime.Hour < 10) lcd.print(' ');
      lcd.print(datetime.Hour);
      lcd.print(':');
      if (datetime.Minute < 10) lcd.print('0');
      lcd.print(datetime.Minute);
      lcd.print(':');
      if (datetime.Second < 10) lcd.print('0');
      lcd.print(datetime.Second);
      lcd.print(" U:");
      lcd.print((uint32_t)(uptime / 60ull / 60ull / 24ull));
      lcd.print(" IT:");
      if (insideT >= 0) lcd.print(' ');
      if (abs(insideT) < 10) lcd.print(' ');
      lcd.print(insideT, 0);
      lcd.write(LCD_DEGREE);

      // Temperature
      lcd.setCursor(0, 1);
      lcd.print("AirT:");
      lcd.print(airT, 1);
      lcd.write(LCD_DEGREE);
      lcd.print(' ');
      // Humidity
      lcd.setCursor(12, 1);
      lcd.print("H:");
      lcd.print(airH, 1);
      lcd.write(LCD_PERCENT);

      // Light level
      lcd.setCursor(0, 2);
      if (lightLevel < 100) lcd.print(' ');
      if (lightLevel < 10) lcd.print(' ');
      lcd.print(lightLevel);
      lcd.write(LCD_LIGHT);
      // Soil Moisture
      lcd.setCursor(5, 2);
      if (soilMoisture < 100) lcd.print(' ');
      if (soilMoisture < 10) lcd.print(' ');
      lcd.print(soilMoisture);
      lcd.write(LCD_FLOWER);
      // Water Level
      lcd.setCursor(10, 2);
      lcd.write(enoughWater ? LCD_WATER : LCD_NO_WATER);
      // Rain
      lcd.setCursor(11, 2);
      lcd.write(raining ? LCD_CLOUD : ' ');

      // Outputs
      lcd.setCursor(0, 3);
      lcd.print(door.state ? "OPEN " : "CLOSE");
      lcd.setCursor(6, 3);
      if (flap1.state < 100) lcd.print(' ');
      if (flap1.state < 10) lcd.print(' ');
      lcd.print(flap1.state);
      lcd.setCursor(10, 3);
      if (flap2.state < 100) lcd.print(' ');
      if (flap2.state < 10) lcd.print(' ');
      lcd.print(flap2.state);
      lcd.setCursor(14, 3);
      lcd.print(light.state ? "ON " : "OFF");
      lcd.setCursor(18, 3);
      lcd.write(pump.state ? LCD_WATER : ' ');
    } else if (page == 1) {
      // Door
      controllable(door, "Door", 1, 0, entry == 0);
      lcd.print(door.manual ? (door.state ? "OPEN " : "CLOSE") : "AUTO ");
      // Flap 1
      controllable(flap1, "Flap 1", 180, 1, entry == 1);
      lcd.print(flap1.manual ? String(flap1.state) + "   " : "AUTO");
      // Flap 2
      controllable(flap2, "Flap 2", 180, 2, entry == 2);
      lcd.print(flap2.manual ? String(flap2.state) + "   " : "AUTO");
      // Light
      controllable(light, "Light", 1, 3, entry == 3);
      lcd.print(light.manual ? (light.state ? "ON  " : "OFF ") : "AUTO");
    } else if (page == 2) {
      // Pump
      controllable(pump, "Pump", settings.wateringTime, 0, entry == 0);
      lcd.print(pump.manual ? (pump.state ? String(pump.state) + "   " : "OFF ") : "AUTO");
    } else if (page == 3) {
      if (!factoryReset) {
        lcd.setCursor(0, 0);
        lcd.print(">Factory Reset");
        if (enc.press()) factoryReset = true;
      } else {
        lcd.setCursor(0, 0);
        lcd.print("Are you sure? ");
        lcd.setCursor(0, 1);
        lcd.print(entry == 0 ? '>' : ' ');
        lcd.print("Yes");
        lcd.setCursor(0, 2);
        lcd.print(entry == 1 ? '>' : ' ');
        lcd.print("No");

        if (enc.press()) {
          if (entry == 0) {
            settings = Settings();
            EEPROM.put(SETTINGS_ADDRESS, settings);
            page = 0;
          }
          entry = 0;
          factoryReset = false;
          lcd.clear();
        }
      }
    }
  }

// Sleep
#ifdef LCD_BACKLIGHT_TIMEOUT
  {
    static uint64_t backlightTimer = 0;
    static bool backlit = true;
    if (enc.action()) {
      if (!backlit) lcd.backlight();
      backlit = true;
      backlightTimer = millis();
    } else if (backlit && millis() - backlightTimer > LCD_BACKLIGHT_TIMEOUT * 1000u) {
      lcd.noBacklight();
      backlit = false;
    }
  }
#endif
}