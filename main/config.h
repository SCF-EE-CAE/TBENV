// Code version
#define FIRMWARE_VERSION "v1_beta"

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

// BAUD rate for Serial port
#define SERIAL_BAUD_RATE 115200

/*
    Sensor configuration, only one of the sensor types should be defined.
*/

#define SENSOR_TYPE_DHT11
// #define SENSOR_TYPE_DHT22
// #define SENSOR_TYPE_DS18B20
// #define SENSOR_TYPE_BME280

// DHT11/DHT22
#define DHT_PIN 2
#define DHT_N_SAMPLINGS 5

// DS18B20
#define DS18B20_PIN 2
#define DS18B20_N_SAMPLINGS 5

// BME280
#define BME_SDA_PIN 0
#define BME_SCL_PIN 2

/*
    General configuration values
*/

#define MAX_VALUES_READ 5   // number of values a sensor can output, for example BME280 has 3: temperature, pressure, humidity
#define DECIMAL_PRECISION 1 // number of digits as decimal places

