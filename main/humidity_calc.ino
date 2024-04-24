// Reference:
// https://carnotcycle.wordpress.com/2012/08/04/how-to-convert-relative-humidity-to-absolute-humidity/
// https://www.engineeringtoolbox.com/humidity-ratio-air-d_686.html

/**
 * @brief Calculate absolute humidity.
 * 
 * @param T Temperature in celsius.
 * @param RH Relative humidity in percent (0-100).
 * @param values JSON object to insert result.
 */
void calc_AH(float T, float RH,
  StaticJsonDocument<JSON_OBJECT_SIZE(MAX_VALUES_READ)>& values) {
  
  float exponential = exp(17.67 * T / (T + 243.5));
  float num = 13.2471 * exponential * RH;
  float den = T + 273.15;
  float AH = num/den;

  values[KEY_ABSOLUTE_HUMIDITY] = serialized(String(AH, DECIMAL_PRECISION));
}

/**
 * @brief Calculate humidity ratio.
 * 
 * @param T Temperature in celsius.
 * @param RH Relative humidity in percent (0-100).
 * @param P Pressure in hPa.
 * @param values JSON object to insert result.
 */
void calc_HR(float T, float RH, float P,
  StaticJsonDocument<JSON_OBJECT_SIZE(MAX_VALUES_READ)>& values) {
  
  float exponential = exp(17.67 * T / (T + 243.5));
  float pw = 0.06112 * exponential * RH; // 6.112 * exp * RH/100   [hPa]
  float HR = 621.98 * pw / (P - pw);     // 0.62198 pw / (pa - pw) [kg/kg] --> *1000 [g/kg]

  values[KEY_HUMIDITY_RATIO] = serialized(String(HR, DECIMAL_PRECISION));
}
