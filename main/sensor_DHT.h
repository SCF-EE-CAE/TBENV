// Sensor Interface definition
#include "sensorInterface.h"

// DHT sensor library
#include "DHT.h"

class Sensor_DHT : public SensorInterface {
public:
  Sensor_DHT(int pin, int type) : dht(pin, type) {} // Constructor initializer list for initializing dht

  void init() override {
    dht.begin();
  }

  bool test() override {
    float temp, hum;
    return read(temp, hum);
  }

  bool readValues(StaticJsonDocument<JSON_OBJECT_SIZE(MAX_VALUES_READ)>& values) override {
    float temp, hum;
    float temp_avg = 0, hum_avg = 0; // Initialize accumulator variables

    Serial.println("Measuring values:");

    for(int sample = 0; sample < DHT_N_SAMPLINGS; sample++) {
      if(!read(temp, hum))
        return false;

      Serial.printf("Sample number %d: temp = %.1f, hum = %.1f\n", sample + 1, temp, hum);

      temp_avg += temp / DHT_N_SAMPLINGS;
      hum_avg += hum / DHT_N_SAMPLINGS;
    }

    // save values in values JSON with 2 decimal places
    values["temperature"] = serialized(String(temp_avg, DECIMAL_PRECISION));
    values["humidity"] =    serialized(String(hum_avg,  DECIMAL_PRECISION));;

    Serial.println("Final values:");
    serializeJson(values, Serial);
    Serial.println();

    return true;
  }

private:
  DHT dht; // Member variable declaration

  bool read(float& temperature, float& humidity, int maxAttempts = 3) {
    for(int attempt = 0; attempt < maxAttempts; attempt++) {
      temperature = dht.readTemperature(); // Celsius
      humidity = dht.readHumidity();
      delay(2100); // minimum delay of 2s

      if(!isnan(temperature) && !isnan(humidity)) {
        return true; // Sensor read successful
      } else {
        Serial.println("Failed to read DHT sensor, retrying...");
      }
    }

    Serial.println("Unable to read DHT sensor after multiple attempts.");
    return false; // All attempts failed
  }
};
