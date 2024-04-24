// Sensor Interface definition
#include "sensorInterface.h"

// DS18B20 ds libraries
#include <OneWire.h>
#include <DallasTemperature.h>

class Sensor_DS18B20 : public SensorInterface {
public:
  Sensor_DS18B20(int pin) : oneWire(pin), ds(&oneWire) {} // Constructor initializer of sensor object

  void init() override {
    ds.begin();
  }

  bool test() override {
    float temp;
    return read(temp);
  }

  bool readValues(StaticJsonDocument<JSON_OBJECT_SIZE(MAX_VALUES_READ)>& values) override {
    float temp;
    float temp_avg = 0;
    int samplesRead = 0;

    Serial.println("Measuring values:");

    for (int sample = 0; sample < DS18B20_N_SAMPLINGS; sample++) {
      if (!read(temp))
        continue;

      Serial.printf("Sample number %d: temp = %.1f\n", sample + 1, temp);

      temp_avg += temp;

      samplesRead++;
    }

    if(samplesRead == 0)
      return false;

    temp_avg /= samplesRead; // Calculate average temperature

    // Save temperature values in values JSON with defined number of decimal places
    values[KEY_TEMPERATURE] = serialized(String(temp_avg, DECIMAL_PRECISION));

    Serial.println("Final values:");
    serializeJson(values, Serial);
    Serial.println();

    return samplesRead == DS18B20_N_SAMPLINGS;
  }

private:
  OneWire oneWire;
  DallasTemperature ds;

  bool read(float& temperature, int maxAttempts = 3) {
    for (int attempt = 0; attempt < maxAttempts; attempt++) {
      ds.requestTemperatures();

      float tempC = ds.getTempCByIndex(0);

      if (tempC != DEVICE_DISCONNECTED_C) { // Check for valid temperature reading
        temperature = tempC;
        Serial.println("Temperature reading successful.");
        return true;
      }

      Serial.println("Error: Failed to get valid temperature reading.");
      delay(2000); // Delay for 2 seconds before next attempt
    }

    Serial.println("Error: Failed after three attempts.");
    return false; // Failed after three attempts or invalid temperature reading
  }
};
