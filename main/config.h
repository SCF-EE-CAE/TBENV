// Code version
#define FIRMWARE_VERSION "v1"

// WiFi Credentials
#define WIFI_SSID     ""
#define WIFI_PASSWORD ""

// Thingsboard address and port
#define TB_SERVER_ADDRESS ""
#define TB_SERVER_PORT    1883U

// Thingsboard Access Token
#define TB_TOKEN ""

// Size of JSON message buffer
#define TB_MAX_MESSAGE_SIZE	256U

// NTP server address and update interval
#define NTP_SERVER "ntp.cti.gov.br"
#define NTP_UPDATE_INTERVAL 21600UL // in seconds (6 hours)

// BAUD rate for Serial port
#define SERIAL_BAUD_RATE 115200

// Define custom MAC address, if needed. Comment out to use original MAC
// #define CUSTOM_MAC_ADDRESS
// uint8_t customMacAddress[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

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

#define MAX_VALUES_READ 10   // number of values produced and sent as telemetry
                            // 5: temperature, relative humidity, absolute humidity
                            // humidity ratio and pressure
#define DECIMAL_PRECISION 1 // number of digits as decimal places
#define DEFAULT_PRESSURE 950 // hPa

/*
  Key names and units of measurement
*/

constexpr char KEY_TEMPERATURE[]  = "temperature";
constexpr char UNIT_TEMPERATURE[] = "C";

constexpr char KEY_RELATIVE_HUMIDITY[]  = "relativeH";
constexpr char UNIT_RELATIVE_HUMIDITY[] = "%";

constexpr char KEY_ABSOLUTE_HUMIDITY[]  = "absoluteH";
constexpr char UNIT_ABSOLUTE_HUMIDITY[] = "g/m3";

constexpr char KEY_HUMIDITY_RATIO[]  = "ratioH";
constexpr char UNIT_HUMIDITY_RATIO[] = "g/kg";

constexpr char KEY_PRESSURE[]  = "pressure";
constexpr char UNIT_PRESSURE[] = "hPa";
