#include <ESP8266WiFi.h>

// NTP libraries
#include <NTPClient.h>
#include <WiFiUdp.h>

#define WIFI_SSID     ""
#define WIFI_PASSWORD ""
#define NTP_SERVER    "a.st1.ntp.br"

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER);

void WiFi_connect() {
  // Check if its already connected
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Connecting to AP, SSID: ");
  Serial.println(WIFI_SSID);

  // Set station mode
  WiFi.mode(WIFI_STA);

  // Attempting to establish a connection to the given WiFi network
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int8_t ret;
  if ((ret = WiFi.waitForConnectResult()) != WL_CONNECTED) {
    Serial.printf("Connection Error, status_code = %d\n", ret);
    Serial.println("Resetting in 5 seconds...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("Connected to AP.");
}

void setup() {
  Serial.begin(115200);

  WiFi_connect();

  // Initialize NTP client
  timeClient.begin();

  // Get time for the first time, timesout in 30s after start
  while(!timeClient.isTimeSet()) {
    Serial.println("Waiting for time to be set by NTP.");
    if(millis() > 30000) {
      Serial.println("Taking too long. Reporting error and restarting.");
      ESP.restart();
    }
    timeClient.update();
    delay(1000);
  }
  Serial.println("NTP time set successfully.");
}

int timeRemaining() {
  int seconds = timeClient.getSeconds();
  int remaining = seconds % 30;
  remaining = (remaining == 0) ? remaining : 30 - remaining;

  return remaining;
}

void loop() {

  delay((timeRemaining() - 1) * 1000);
  while(timeRemaining() != 0);

  Serial.printf("%u\r\n", micros());

  while(timeRemaining() == 0);
}
