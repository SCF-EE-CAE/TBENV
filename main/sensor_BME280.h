// Sensor Interface definition
#include "sensorInterface.h"

// BME280 libraries. I2C, sensor
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

class Sensor_BME280 : public SensorInterface {
public:
  // Constructor with SDA and SCL parameters
  Sensor_BME280(int sdaPin, int sclPin) : sdaPin(sdaPin), sclPin(sclPin) {}

  void init() override {
    Wire.begin(sdaPin, sclPin); // Init I2C

    if(!bme.begin(0x76, &Wire)) return;

    // set sampling to max, filter, and forced mode.
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X16,  // temperature
                    Adafruit_BME280::SAMPLING_X16,  // pressure
                    Adafruit_BME280::SAMPLING_X16,  // humidity
                    Adafruit_BME280::FILTER_X16);
  }

  bool test() override {
    float temp, hum, pres;
    return read(temp, hum, pres);
  }

  bool readValues(StaticJsonDocument<JSON_OBJECT_SIZE(MAX_VALUES_READ)>& values) override {
    float temp, hum, pres;

    if(!read(temp, hum, pres))
      return false;
    
    // save values in values JSON with defined number of decimal places
    values[KEY_TEMPERATURE] = serialized(String(temp, DECIMAL_PRECISION));
    values[KEY_HUMIDITY] =    serialized(String(hum,  DECIMAL_PRECISION));
    values[KEY_PRESSURE] =    serialized(String(pres, DECIMAL_PRECISION));

    Serial.println("Final values:");
    serializeJson(values, Serial);
    Serial.println();

    return true;
  }

private:
  Adafruit_BME280 bme;
  int sdaPin;
  int sclPin;

  bool read(float& temperature, float& humidity, float& pressure, int maxAttempts = 3) {
    for (int attempt = 0; attempt < maxAttempts; attempt++) {
      bme.takeForcedMeasurement();

      temperature = bme.readTemperature(); // Celsius
      pressure = bme.readPressure() / 100.0F;
      humidity = bme.readHumidity();

      if (!isnan(temperature) && !isnan(pressure) && !isnan(humidity) &&
          (temperature > 0 && temperature < 100)) {
        return true; // Sensor read successful
      } else {
        Serial.println("Failed to read BME sensor, retrying in 2 seconds...");
        delay(2000);
      }
    }

    Serial.println("Unable to read BME sensor after multiple attempts.");
    return false; // All attempts failed
  }
};
