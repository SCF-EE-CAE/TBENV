// WiFi library for ESP8266
#include <ESP8266WiFi.h>

// MQTT and Thingsboard libraries
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>

// NTP libraries
#include <NTPClient.h>
#include <WiFiUdp.h>

// Configuration file
#include "config.h"

// Selected sensor setup and libraries
#ifdef SENSOR_TYPE_DHT11

  #include "sensor_DHT11.h"
  SensorInterface sensor = Sensor_DHT11();
  #define SENSOR_TYPE_STR "DHT11"

#elif defined(SENSOR_TYPE_DHT22)

  #include "sensor_DHT22.h"
  SensorInterface sensor = Sensor_DHT22();
  #define SENSOR_TYPE_STR "DHT22"

#elif defined(SENSOR_TYPE_DS18B20)

  #include "sensor_DS18B20.h"
  SensorInterface sensor = Sensor_DS18B20();
  #define SENSOR_TYPE_STR "DS18B20"

#elif defined(SENSOR_TYPE_BME280)

  #include "sensor_BME280.h"
  SensorInterface sensor = Sensor_BME280();
  #define SENSOR_TYPE_STR "BME280"

#else
  #error "No sensor type defined or unsupported sensor type selected."
#endif

// Asserts only one sensor type was selected
#if defined(SENSOR_TYPE_DHT11) + defined(SENSOR_TYPE_DHT22) + defined(SENSOR_TYPE_DS18B20) + defined(SENSOR_TYPE_BME280) > 1
  #error "Only one sensor type should be defined at a time."
#endif


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
