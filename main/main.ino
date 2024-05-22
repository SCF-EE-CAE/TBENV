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

// Track last NTP update sucessfull
uint64_t lastNtpUpdateTs;

int timeRemaining() {
  int seconds = timeClient.getSeconds();
  int remaining = seconds % 30;
  remaining = (remaining == 0) ? remaining : 30 - remaining;

  return remaining;
}

inline void sendSensorStatus(bool status, bool valuesToSend) {
  // Track last sensor status reported. true = OK, false otherwise
  static bool lastSensorStatus = true;

  if(!status) {
    if(!valuesToSend) {
      // error status and no output in values JSON
      Serial.println("Sensor error.");
      sendStatus("SENSOR", "ERROR", false);
    }
    else {
      // error status but produced values
      Serial.println("Sensor warning.");
      sendStatus("SENSOR", "WARNING", false);
    }

    lastSensorStatus = false;
  }
  else if(!lastSensorStatus) {
    // send OK status only if last status sent was not OK
    sendStatus("SENSOR", "OK", false);
    lastSensorStatus = true;
  }
}

inline void sendTimestampedData(uint64_t timestamp) {
  // build telemetry json
  telemetry["ts"] = timestamp * 1000UL; // to ms
  telemetry["values"] = values;

  Serial.println("Sending JSON to server");

  // send telemetry json
  tb.sendTelemetryJson(telemetry, measureJson(telemetry) + 1);
}

inline void handleNTPupdate() {
  uint64_t timestamp = timeClient.getEpochTime();
  uint64_t timeSinceLastUpdate = timestamp - lastNtpUpdateTs;
  
  if(timeSinceLastUpdate > NTP_UPDATE_INTERVAL) {
    bool status = timeClient.forceUpdate();

    if(status) {
      // Report update success
      lastNtpUpdateTs = timeClient.getEpochTime();
      sendStatus("NTP", "OK", false);
    }
    else if(timeSinceLastUpdate >= 3 * NTP_UPDATE_INTERVAL) {
      // Report WARNING, with time (in hours) with no update
      char auxBuffer[30];
      snprintf(auxBuffer, sizeof(auxBuffer), "WARNING_%dh", timeSinceLastUpdate/3600);
      sendStatus("NTP", auxBuffer, false);
    }
  }
}

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

  lastNtpUpdateTs = timeClient.getEpochTime(); // save time of NTP set

  // Initialize and test sensor
  sensor.init();
  
  if(!sensor.test()) {
    Serial.println("Sensor error.");
    sendStatus("SENSOR", "INIT_ERROR", true);
    ESP.restart();
  }
  Serial.println("Sensor OK.");
  sendStatus("SENSOR", "OK", false);

  // Send units of measurement of data produced by the sensor
  if(!sendUnitsOfMeasurement()) {
    Serial.println("Error sending units.");
    sendStatus("UNITS", "ERROR", true);
    ESP.restart();
  }
  Serial.println("Send units OK.");
  sendStatus("UNITS", "OK", false);
}

void loop() {
  // Wait for correct instant in time
  delay((timeRemaining() - 1) * 1000);
  while(timeRemaining() != 0);

  // Save current timestamp in seconds
  uint64_t timestamp = timeClient.getEpochTime();

  // Clear JSON objects
  values.clear();
  telemetry.clear();

  // Read sensor and check for returned values
  bool status = sensor.readValues(values);
  bool valuesToSend = !values.isNull();
  
  // Reestablish connection with TB, if lost
  connect();

  // Send sensor status to TB
  sendSensorStatus(status, valuesToSend);

  // if some output was produced send data to TB
  if(valuesToSend) sendTimestampedData(timestamp);

  // Check for time with no update from NTP server and update
  handleNTPupdate();

  // Wait for second end, if finished in less than one second
  while(timeRemaining() == 0);
}
