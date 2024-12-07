
# ESP32 Weather Station

A Wi-Fi enabled weather station using ESP32, OpenWeather API, and DHT22 sensor.

## Features
- Displays local temperature, humidity, and time on an OLED screen.
- Fetches weather data from the OpenWeather API.
- Allows configuration of Wi-Fi settings and weather API via a web interface.

## Setup
1. Clone the repository.
2. Set up your ESP32 with the Arduino IDE.
3. Modify the code with your Wi-Fi credentials and OpenWeather API key.
4. Upload the code to the ESP32 and power it on.
5. Access the weather station's web interface to configure and monitor the system.

## Libraries Required
- DHT sensor library
- ESPAsyncWebServer
- NTPClient
- U8g2lib
