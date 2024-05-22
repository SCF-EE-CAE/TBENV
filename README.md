# TBENV - v2

Código do módulo ambiental do projeto, suporta os sensores DHT, BME280 e DS18B20. Código principal presente na pasta _main_ e códigos de teste na pasta _aux_.

A configuração do sensor utilizado, pino, rede e senha WiFi e muitas outras são feitas no arquivo _main/config.h_.

## Dependências

As seguintes bibliotecas são dependências do projeto. Atenção para as versões, algumas dependências podem atualizar e ficarem incompatíveis com o código final ou com alguma outra dependência. As versões mostradas aqui foram testadas e o funcionamento confirmado.

### Thingsboard

- ThingsBoard 0.12.2
- ArduinoHttpClient 0.6.0
- ArduinoJson 6.21.4
- TBPubSubClient 2.9.3
- Seeed_Arduino_mbedtls 3.0.1

### NTP (Sincronização com o tempo da internet)

- NTPClient 3.2.1

### Sensores do tipo DHT

- DHT sensor library 1.4.6
- Adafruit Unified Sensor 1.1.14

### Sensor DS18B20

- DallasTemperature 3.9.0
- OneWire 2.3.7

### Sensor BME280

- Adafruit BME280 Library 2.2.4
- Adafruit BusIO 1.16.1
- Adafruit Unified Sensor 1.1.14

