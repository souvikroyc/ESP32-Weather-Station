#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Preferences.h>

// OLED Display
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// DHT22 Sensor
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Web Server
AsyncWebServer server(80);

// Wi-Fi Variables
const char* AP_SSID = "ESP32_WeatherStation";
const char* AP_PASSWORD = "123456789";
String clientSSID = "";
String clientPassword = "";
String apiKey = "";
String city = "";

// NTP and Time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000);

// Weather API (Updated with user input for city and API key)
String weatherAPI = "";
float onlineTemp = 0.0;
String weatherDescription = "";

// Sensor Data
float localTemp = 0.0;
float localHumidity = 0.0;

// Page Auto-scrolling
int page = 0;
unsigned long lastPageSwitch = 0;

// Preferences for saving data
Preferences preferences;

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Initialize OLED
  u8g2.begin();

  // Start Access Point
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.print("Access Point IP: ");
  Serial.println(WiFi.softAPIP());

  // Setup Web Server
  setupWebServer();

  // Start NTP
  timeClient.begin();

  // Load Wi-Fi credentials and API key from Preferences
  preferences.begin("wifi_config", false);  // "wifi_config" is the namespace
  clientSSID = preferences.getString("ssid", "");
  clientPassword = preferences.getString("password", "");
  apiKey = preferences.getString("apikey", "");
  city = preferences.getString("city", "");
  preferences.end();

  // If saved Wi-Fi credentials exist, try connecting to Wi-Fi
  if (clientSSID.length() > 0 && clientPassword.length() > 0) {
    WiFi.begin(clientSSID.c_str(), clientPassword.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("\nWi-Fi Connected!");
    fetchWeatherData();
  }
}

void loop() {
  // Update time and sensor data
  timeClient.update();
  localTemp = dht.readTemperature();
  localHumidity = dht.readHumidity();

  // Auto-scroll OLED pages
  if (millis() - lastPageSwitch > 5000) {
    page = (page + 1) % 4; // Cycle through 4 pages
    updateOLED(page);
    lastPageSwitch = millis();
  }
}

// Web Server Setup
void setupWebServer() {
  // Main page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<html><head><title>ESP32 Weather Station</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; background-color: #f4f4f4; color: #333; text-align: center; padding: 20px; }";
    html += "h1 { color: #007bff; font-size: 2em; }";
    html += "p { font-size: 1.2em; }";
    html += "form { display: block; margin-top: 20px; padding: 10px; border-radius: 10px; background-color: #fff; box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1); }";
    html += "input[type='text'], input[type='password'], select { padding: 10px; margin: 10px 0; width: 250px; font-size: 1.1em; border-radius: 5px; border: 1px solid #ccc; }";
    html += "button { padding: 12px 25px; background-color: #28a745; color: white; border: none; font-size: 1.2em; border-radius: 5px; cursor: pointer; }";
    html += "button:hover { background-color: #218838; }";
    html += "table { margin-top: 20px; border-spacing: 10px; width: 100%; text-align: left; background-color: #f9f9f9; border-radius: 10px; padding: 10px; }";
    html += "td { padding: 10px; font-size: 1.1em; }";
    html += "td.label { font-weight: bold; }";
    html += "td.value { font-style: italic; color: #007bff; }";
    html += "</style>";
    html += "</head><body>";
    html += "<h1>ESP32 Weather Station</h1>";

    // Display Wi-Fi status and connection information
    html += "<h2>Current Wi-Fi Status</h2>";
    if (WiFi.status() == WL_CONNECTED) {
      html += "<table><tr><td class='label'>Connected Wi-Fi:</td><td class='value'>" + WiFi.SSID() + "</td></tr>";
      html += "<tr><td class='label'>IP Address:</td><td class='value'>" + WiFi.localIP().toString() + "</td></tr></table>";
      html += "<p><a href='/disconnect'>Disconnect Wi-Fi</a> or change Wi-Fi settings below.</p>";
    } else {
      html += "<table><tr><td class='label'>Current Wi-Fi:</td><td class='value'>Not connected</td></tr></table>";
    }

    // Display OpenWeather API status
    html += "<h2>OpenWeather API Status</h2>";
    if (apiKey.length() > 0 && city.length() > 0) {
      html += "<table><tr><td class='label'>City:</td><td class='value'>" + city + "</td></tr>";
      html += "<tr><td class='label'>API Key:</td><td class='value'>Configured</td></tr></table>";
      html += "<p><a href='/change_weather'>Change Weather API or City</a> below.</p>";
    } else {
      html += "<table><tr><td class='label'>City:</td><td class='value'>Not configured</td></tr>";
      html += "<tr><td class='label'>API Key:</td><td class='value'>Not configured</td></tr></table>";
    }

    // Form for Wi-Fi and Weather API configuration
    html += "<h3>Configure Wi-Fi and Weather API</h3>";
    html += "<form action='/connect' method='POST'>";
    html += "<table><tr><td class='label'>Wi-Fi SSID:</td><td><select name='ssid'>";
    
    // List available Wi-Fi networks
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++) {
      html += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + "</option>";
    }
    
    html += "</select></td></tr>";
    html += "<tr><td class='label'>Password:</td><td><input type='password' name='password'></td></tr>";
    html += "<tr><td class='label'>City Name:</td><td><input type='text' name='city' placeholder='Enter City'></td></tr>";
    html += "<tr><td class='label'>API Key:</td><td><input type='text' name='apikey' placeholder='Enter OpenWeather API Key'></td></tr></table>";
    html += "<button type='submit'>Save Settings</button>";
    html += "</form>";
    
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // Wi-Fi connection and Weather API setup
  server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true) && request->hasParam("password", true) &&
        request->hasParam("city", true) && request->hasParam("apikey", true)) {
      clientSSID = request->getParam("ssid", true)->value();
      clientPassword = request->getParam("password", true)->value();
      city = request->getParam("city", true)->value();
      apiKey = request->getParam("apikey", true)->value();
      
      weatherAPI = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric";
      
      request->send(200, "text/html", "<html><body><h1>Connecting...</h1></body></html>");
      
      // Save Wi-Fi and API information to Preferences
      preferences.begin("wifi_config", false);
      preferences.putString("ssid", clientSSID);
      preferences.putString("password", clientPassword);
      preferences.putString("city", city);
      preferences.putString("apikey", apiKey);
      preferences.end();
      
      connectToWiFi();
    } else {
      request->send(400, "text/html", "Missing SSID, Password, City or API Key");
    }
  });

  server.begin();
}

// Connect to Wi-Fi and fetch weather data
void connectToWiFi() {
  WiFi.softAPdisconnect(true);
  WiFi.begin(clientSSID.c_str(), clientPassword.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
  fetchWeatherData();
}

// Fetch online weather data
void fetchWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(weatherAPI);
    int httpCode = http.GET();
    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      onlineTemp = doc["main"]["temp"];
      weatherDescription = doc["weather"][0]["description"].as<String>();
    }
    http.end();
  }
}

// Update OLED pages
void updateOLED(int page) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  switch (page) {
    case 0: // Local sensor data
      u8g2.drawStr(0, 10, "Local Weather:");
      u8g2.setCursor(0, 30);
      u8g2.print("Temp: ");
      u8g2.print(localTemp);
      u8g2.print(" C");
      u8g2.setCursor(0, 50);
      u8g2.print("Humidity: ");
      u8g2.print(localHumidity);
      u8g2.print(" %");
      break;
    case 1: // Online weather data
      u8g2.drawStr(0, 10, "Online Weather:");
      u8g2.setCursor(0, 30);
      u8g2.print("Temp: ");
      u8g2.print(onlineTemp);
      u8g2.print(" C");
      u8g2.setCursor(0, 50);
      u8g2.print(weatherDescription);
      break;
    case 2: // Time
      u8g2.drawStr(0, 10, "Current Time:");
      u8g2.setCursor(0, 30);
      u8g2.print(timeClient.getFormattedTime());
      break;
    case 3: // Wi-Fi info
      u8g2.drawStr(0, 10, "Wi-Fi Info:");
      u8g2.setCursor(0, 30);
      u8g2.print("SSID: ");
      u8g2.print(WiFi.SSID());
      u8g2.setCursor(0, 50);
      u8g2.print("IP: ");
      u8g2.print(WiFi.localIP());
      break;
  }
  u8g2.sendBuffer();
}
