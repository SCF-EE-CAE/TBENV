// Code version
#define FIRMWARE_VERSION "beta"

// WiFi Credentials
#define WIFI_SSID     ""
#define WIFI_PASSWORD ""

// Thingsboard address and port
#define TB_SERVER_ADDRESS ""
#define TB_SERVER_PORT    1883U

// Thingsboard Access Token
#define TB_TOKEN ""

// Size of JSON message buffer
#define TB_MAX_MESSAGE_SIZE	128U

// NTP server address
#define NTP_SERVER ""

/*
    Sensor configuration, only one of the sensor types should be defined.
*/

#define SENSOR_TYPE_DHT11
// #define SENSOR_TYPE_DHT22
// #define SENSOR_TYPE_DS18B20
// #define SENSOR_TYPE_BME280
