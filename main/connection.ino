/**
 * @brief Establishes a WiFi connection if not already connected.
 * 
 * This function checks if the device is already connected to a WiFi network.
 * If not, it attempts to establish a connection using the provided SSID
 * and password. It waits for the connection result and restarts the device
 * if the connection fails.
 * 
 * @remarks This function assumes that global variables WIFI_SSID and WIFI_PASSWORD
 *          contain the SSID and password of the WiFi network, respectively.
 */
void WiFi_connect() {
  // Check if its already connected
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Connecting to AP, SSID: ");
  Serial.println(WIFI_SSID);

  // Attempting to establish a connection to the given WiFi network
  WiFi.mode(WIFI_STA);
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

/**
 * @brief Connects to the ThingsBoard server.
 * 
 * This function checks if the device is already connected to the ThingsBoard
 * server. If not, it attempts to establish a connection using the server
 * address and access token. It waits for the connection result and restarts
 * the device if the connection fails.
 * 
 * @remarks This function assumes that global variables TB_SERVER_ADDRESS, TB_SERVER_PORT and 
 *          TB_TOKEN contain the server address, port and access token for ThingsBoard, respectively.
 */
void TB_connect() {
  if(tb.connected()) return;

  Serial.printf("Connecting to Thingsboard server: (%s) with access token\n", TB_SERVER_ADDRESS);
  
  if(!tb.connect(TB_SERVER_ADDRESS, TB_TOKEN, TB_SERVER_PORT)) {
    Serial.println("Failed to connect.");
    Serial.println("Resetting in 5 seconds...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("Connected to Thingsboard.");
}

/**
 * @brief Connects to both WiFi and ThingsBoard servers.
 * 
 * This function orchestrates the connection process by first establishing
 * a WiFi connection and then connecting to the ThingsBoard server.
 */
inline void connect() {
  // WiFi connection
  WiFi_connect();

  // Thingsboard connection
  TB_connect();
}
