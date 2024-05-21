/**
 * @brief Sends system information to the ThingsBoard server.
 * 
 * This function retrieves the device's firmware version, local IP address,
 * MAC address, and sensor type information and sends it to the ThingsBoard
 * server as attributes.
 * 
 * @remarks The firmware version, sensor type, and global variables WIFI_SSID,
 *          WIFI_PASSWORD, TB_SERVER_ADDRESS, TB_TOKEN, and SENSOR_TYPE_STR must
 *          be properly defined for this function to work correctly.
 * @remarks The IP address and MAC address are copied into local buffers before
 *          being added to the attributes array to ensure proper memory management.
 */
void sendSystemInfo() {
  char ipBuffer[16];
  char macBuffer[18];

  strcpy(ipBuffer,  WiFi.localIP().toString().c_str());
  strcpy(macBuffer, WiFi.macAddress().c_str());

  const int attribute_items = 4;
  Attribute attributes[attribute_items] = {
    {"firmwareVersion", FIRMWARE_VERSION},
    {"ip", ipBuffer},
    {"mac", macBuffer},
    {"sensorType", SENSOR_TYPE_STR},
  };

  tb.sendAttributes(attributes, attribute_items);
}

/**
 * @brief Sends units of measurement to the ThingsBoard server.
 * 
 * This function retrieves sensor values and sends their corresponding units of measurement
 * to the ThingsBoard server as attributes.
 * 
 * @return true if the units of measurement were successfully sent, false otherwise.
 * 
 * @remarks The global variables SENSOR_TYPE_STR, KEY_TEMPERATURE, KEY_HUMIDITY, 
 *          KEY_PRESSURE, UNIT_TEMPERATURE, UNIT_HUMIDITY, and UNIT_PRESSURE must be 
 *          properly defined for this function to work correctly.
 * @remarks The function relies on an external sensor object named 'sensor' and an 
 *          external ThingsBoard object named 'tb' for retrieving sensor values and 
 *          sending attribute data respectively.
 */
bool sendUnitsOfMeasurement() {
  char auxBuffer[30];

  bool status = sensor.readValues(values);

  if(!status)
    return false;

  for (JsonPair pair : values.as<JsonObject>()) {
      if(strcmp(pair.key().c_str(), KEY_TEMPERATURE) == 0) {
        snprintf(auxBuffer, sizeof(auxBuffer), "%s_unit", KEY_TEMPERATURE);
        if(!tb.sendAttributeData(auxBuffer, UNIT_TEMPERATURE))
          return false;
      }

      else if(strcmp(pair.key().c_str(), KEY_RELATIVE_HUMIDITY) == 0) {
        snprintf(auxBuffer, sizeof(auxBuffer), "%s_unit", KEY_RELATIVE_HUMIDITY);
        if(!tb.sendAttributeData(auxBuffer, UNIT_RELATIVE_HUMIDITY))
          return false;
      }

      else if(strcmp(pair.key().c_str(), KEY_ABSOLUTE_HUMIDITY) == 0) {
        snprintf(auxBuffer, sizeof(auxBuffer), "%s_unit", KEY_ABSOLUTE_HUMIDITY);
        if(!tb.sendAttributeData(auxBuffer, UNIT_ABSOLUTE_HUMIDITY))
          return false;
      }

      else if(strcmp(pair.key().c_str(), KEY_HUMIDITY_RATIO) == 0) {
        snprintf(auxBuffer, sizeof(auxBuffer), "%s_unit", KEY_HUMIDITY_RATIO);
        if(!tb.sendAttributeData(auxBuffer, UNIT_HUMIDITY_RATIO))
          return false;
      }

      else if(strcmp(pair.key().c_str(), KEY_PRESSURE) == 0) {
        snprintf(auxBuffer, sizeof(auxBuffer), "%s_unit", KEY_PRESSURE);
        if(!tb.sendAttributeData(auxBuffer, UNIT_PRESSURE))
          return false;
      }

      else return false; // unrecognized value
  }

  return true;
}

/**
 * @brief Sends status data to the ThingsBoard server.
 * 
 * This function constructs an attribute name using the provided label
 * and sends the status data to the ThingsBoard server. Optionally, it
 * can introduce a delay to wait for delivery.
 * 
 * @param label The label for the status attribute.
 * @param status The status data to send.
 * @param delay Flag indicating whether to introduce a delay for delivery.
 * 
 * @remarks The attribute name is constructed by appending "_status" to the label.
 * @remarks If delay is true, a delay of 5000 milliseconds (5 seconds) is introduced after the message is sent.
 */
void sendStatus(const char *label, const char *status, bool wait) {
  char attributeName[25];

  snprintf(attributeName, sizeof(attributeName), "%s_status", label);
  tb.sendAttributeData(attributeName, status);

  if(wait) // delay to wait for delivery
    delay(5000); 
}
