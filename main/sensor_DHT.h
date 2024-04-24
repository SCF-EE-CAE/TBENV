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
    float temp_avg = 0, hum_avg = 0;
    int samplesRead = 0;

    Serial.println("Measuring values:");

    for(int sample = 0; sample < DHT_N_SAMPLINGS; sample++) {
      if(!read(temp, hum))
        continue;

      Serial.printf("Sample number %d: temp = %.1f, hum = %.1f\n", sample + 1, temp, hum);

      temp_avg += temp;
      hum_avg += hum;

      samplesRead++;
    }

    if(samplesRead == 0)
      return false;

    temp_avg /= samplesRead;
    hum_avg  /= samplesRead;

    // save values in values JSON with defined number of decimal places
    values["temperature"] = serialized(String(temp_avg, DECIMAL_PRECISION));
    values["humidity"] =    serialized(String(hum_avg,  DECIMAL_PRECISION));

    Serial.println("Final values:");
    serializeJson(values, Serial);
    Serial.println();

    return samplesRead == DHT_N_SAMPLINGS;
  }

private:
  DHT dht;

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
