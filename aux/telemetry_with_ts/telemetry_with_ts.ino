#include <ESP8266WiFi.h>

#define THINGSBOARD_ENABLE_PROGMEM 0

#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>

constexpr char WIFI_SSID[] = "";
constexpr char WIFI_PASSWORD[] = "";

constexpr char TOKEN[] = "";

constexpr char THINGSBOARD_SERVER[] = "";

constexpr uint16_t THINGSBOARD_PORT = 1883U;

constexpr uint16_t MAX_MESSAGE_SIZE = 128U;

constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;


// Initalize the Mqtt client and Thingsboard instance
WiFiClient espClient;
Arduino_MQTT_Client mqttClient(espClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);


/// @brief Initalizes WiFi connection,
// will endlessly delay until a connection has been successfully established
void InitWiFi() {
  Serial.println("Connecting to AP ...");
  // Attempting to establish a connection to the given WiFi network
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    // Delay 500ms until a connection has been successfully established
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

/// @brief Reconnects the WiFi uses InitWiFi if the connection has been removed
/// @return Returns true as soon as a connection has been established again
bool reconnect() {
  // Check to ensure we aren't connected yet
  const wl_status_t status = WiFi.status();
  if (status == WL_CONNECTED) {
    return true;
  }

  // If we aren't establish a new connection to the given WiFi network
  InitWiFi();
  return true;
}

void setup() {
  // If analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
  // Initalize serial connection for debugging
  Serial.begin(SERIAL_DEBUG_BAUD);
  delay(1000);
  InitWiFi();
}

void loop() {
  delay(1000);

  if (!reconnect()) {
    return;
  }

  if (!tb.connected()) {
    // Reconnect to the ThingsBoard server,
    // if a connection was disrupted or has not yet been established
    Serial.printf("Connecting to: (%s) with token (%s)\n", THINGSBOARD_SERVER, TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
      Serial.println("Failed to connect");
      return;
    }
  }

  // allocate the memory for the document
  StaticJsonDocument<JSON_OBJECT_SIZE(2)> values;
  StaticJsonDocument<JSON_OBJECT_SIZE(4)> telemetry;

  values["temp"] = 42.2;
  values["hum"] = 70;
  telemetry["ts"] = 1713279600000;
  telemetry["values"] = values;

  char telemetry_str[200];
  serializeJson(telemetry, telemetry_str);

  Serial.println("Sending JSON string...");
  Serial.println(telemetry_str);
  
  bool status = tb.sendTelemetryJson((const char*)telemetry_str);
  Serial.println(status ? "success" : "fail");

  tb.loop();
}
