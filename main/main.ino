// WiFi library for ESP8266
#include <ESP8266WiFi.h>

// MQTT and Thingsboard libraries
#define THINGSBOARD_ENABLE_PROGMEM 0 // ESP8266 does not support Strings in flash memory
#define THINGSBOARD_ENABLE_DEBUG 0 // debug mode select
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>

// NTP libraries
#include <NTPClient.h>
#include <WiFiUdp.h>

// Configuration file
#include "config.h"

// Selected sensor setup and libraries
#ifdef SENSOR_TYPE_DHT11

  #include "sensor_DHT.h"
  Sensor_DHT sensor(DHT_PIN, DHT11);
  #define SENSOR_TYPE_STR "DHT11"

#elif defined(SENSOR_TYPE_DHT22)

  #include "sensor_DHT.h"
  Sensor_DHT sensor(DHT_PIN, DHT22);
  #define SENSOR_TYPE_STR "DHT22"

#elif defined(SENSOR_TYPE_DS18B20)

  #include "sensor_DS18B20.h"
  Sensor_DS18B20 sensor(DS18B20_PIN);
  #define SENSOR_TYPE_STR "DS18B20"

#elif defined(SENSOR_TYPE_BME280)

  #include "sensor_BME280.h"
  Sensor_BME280 sensor(BME_SDA_PIN, BME_SCL_PIN);
  #define SENSOR_TYPE_STR "BME280"

#else
  #error "No sensor type defined or unsupported sensor type selected."
#endif

// Asserts only one sensor type was selected
#if defined(SENSOR_TYPE_DHT11) + defined(SENSOR_TYPE_DHT22) + defined(SENSOR_TYPE_DS18B20) + defined(SENSOR_TYPE_BME280) > 1
  #error "Only one sensor type should be defined at a time."
#endif

// Initalize the Mqtt client and Thingsboard instance
WiFiClient espClient;
Arduino_MQTT_Client mqttClient(espClient);
ThingsBoard tb(mqttClient, TB_MAX_MESSAGE_SIZE);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER);

// JSON objects for telemetry data
StaticJsonDocument<JSON_OBJECT_SIZE(MAX_VALUES_READ)> values;
StaticJsonDocument<JSON_OBJECT_SIZE(MAX_VALUES_READ + 1)> telemetry;

// Track last data send status
bool lastDataSent = true;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  delay(500); // wait for Serial begin

  // Show MAC
  Serial.printf("\n\nMAC address: %s\n", WiFi.macAddress().c_str());

  // Connect to WiFi and Thingsboard
  connect();
  
  // Send system info to TB (attributes)
  sendSystemInfo();

  // Initialize NTP client
  timeClient.begin();

  // Get time for the first time, timesout in 30s after start
  // Send to Thingsboard status of NTP time
  while(!timeClient.isTimeSet()) {
    Serial.println("Waiting for time to be set by NTP.");
    if(millis() > 30000) {
      Serial.println("Taking too long. Reporting error and restarting.");
      sendStatus("NTP", "ERROR", true);
      ESP.restart();
    }
    timeClient.update();
    delay(1000);
  }
  Serial.println("NTP time set successfully.");
  sendStatus("NTP", "OK", false);

  // Initialize and test sensor
  sensor.init();
  
  if(!sensor.test()) {
    Serial.println("Sensor error.");
    sendStatus("SENSOR", "INIT_ERROR", true);
    ESP.restart();
  }
  Serial.println("Sensor OK.");
  sendStatus("SENSOR", "OK", false);
}

void loop() {
  delay(500);

  // Reestablish connection with TB, if lost
  connect();

  // MQTT send / receive routines
  tb.loop();

  // Retries to send last data
  if(!lastDataSent) {
    Serial.printf("Retrying to send data.");
    lastDataSent = tb.sendTelemetryJson(telemetry, measureJson(telemetry) + 1);
  }

  int seconds = timeClient.getSeconds();
  if(seconds == 0 || seconds == 30) {
    // save timestamp in seconds
    uint64_t timestamp = timeClient.getEpochTime();

    // Clear JSON objects
    values.clear();
    telemetry.clear();

    // read sensor and manage error
    bool status = sensor.readValues(values);
    bool valuesToSend = !values.isNull();
    
    if(!status) {
      if(!valuesToSend) {
        // error status and no output in values JSON
        Serial.println("Sensor error.");
        sendStatus("SENSOR", "ERROR", false);

        lastDataSent = true;
      }
      else {
        // error status but produced values
        Serial.println("Sensor warning.");
        sendStatus("SENSOR", "WARNING", false);
      }
    }
    else
      sendStatus("SENSOR", "OK", false);

    // if some output was produced
    if(valuesToSend) {
      // build telemetry json
      telemetry["ts"] = timestamp * 1000UL; // to ms
      telemetry["values"] = values;

      Serial.println("Sending JSON to server");

      // send telemetry json
      lastDataSent = tb.sendTelemetryJson(telemetry, measureJson(telemetry) + 1);

      Serial.printf("Send %s", lastDataSent ? "success.\n" : "failed, trying again soon.\n");

      // Update internal time with NTP server
      timeClient.update();
    }

    delay(1100); // wait for second finish
  }
}
