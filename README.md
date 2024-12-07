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

## Troubleshooting and Debugging

### 1. **Wi-Fi Connection Issues**
   - **Problem:** ESP32 is unable to connect to Wi-Fi.
   - **Solution:** 
     - Ensure that the SSID and password in the code match your router's details.
     - If you're using the web interface to set the Wi-Fi, double-check the Wi-Fi credentials you've entered.
     - Make sure your Wi-Fi router supports 2.4 GHz (ESP32 may not connect to 5 GHz networks).
     - Check for any special characters in the SSID or password that may cause issues.

   - **How to Debug:**
     - Use the `Serial.println()` statements to print the Wi-Fi status and error messages.
     - Add debugging messages like:
       ```cpp
       Serial.print("Connecting to Wi-Fi...");
       if (WiFi.status() != WL_CONNECTED) {
           Serial.println("Failed to connect");
       }
       ```
     - Open the Serial Monitor to see the connection status.

### 2. **DHT22 Sensor Not Working**
   - **Problem:** The temperature and humidity readings are incorrect or unavailable.
   - **Solution:**
     - Double-check the wiring of the DHT22 sensor. Ensure that the `VCC`, `GND`, and `DATA` pins are properly connected.
     - Ensure you're using the correct pin in the code for the DHT22 sensor.
     - Make sure you have the DHT library installed and updated in the Arduino IDE.
   
   - **How to Debug:**
     - Add debugging in the `loop()` function to check the sensor readings:
       ```cpp
       float temperature = dht.readTemperature();
       float humidity = dht.readHumidity();
       if (isnan(temperature) || isnan(humidity)) {
           Serial.println("Failed to read from DHT sensor!");
       } else {
           Serial.print("Temperature: ");
           Serial.print(temperature);
           Serial.print("°C, Humidity: ");
           Serial.print(humidity);
           Serial.println("%");
       }
       ```
     - Check the Serial Monitor for any error messages related to the sensor.

### 3. **OpenWeather API Issues**
   - **Problem:** Weather data is not being displayed or fetched.
   - **Solution:**
     - Ensure that the OpenWeather API key is valid and correctly entered in the code or the web interface.
     - Make sure that the city name in the code or entered in the web form is correct and matches the format expected by OpenWeather.
     - Verify that you have an active internet connection.
   
   - **How to Debug:**
     - Print the HTTP request status code to ensure the request to OpenWeather is successful:
       ```cpp
       if (httpCode == HTTP_OK) {
           // Successfully fetched data
       } else {
           Serial.print("Error on HTTP request: ");
           Serial.println(httpCode);
       }
       ```
     - Check the Serial Monitor for the HTTP status codes and error messages.

### 4. **OLED Display Not Showing Data**
   - **Problem:** The OLED screen is blank or not displaying the correct information.
   - **Solution:**
     - Ensure the OLED screen is correctly connected to the ESP32. Check the SDA and SCL pins in the code and make sure they match the actual connections.
     - Verify that the `U8g2lib` library is properly installed and up-to-date.
     - Make sure that the ESP32 is powered correctly and that there is no issue with the power supply.
   
   - **How to Debug:**
     - Print debug information to the Serial Monitor to check if the data is being fetched and processed correctly, but not displayed.
     - Try running a basic example code from the `U8g2lib` library to test the OLED display functionality.

### 5. **Web Interface Not Loading**
   - **Problem:** The ESP32 web interface does not load in the browser.
   - **Solution:**
     - Ensure that your device is connected to the same network as the ESP32.
     - Check if the IP address displayed in the Serial Monitor is correct.
     - Try accessing the web interface via `http://<ESP32_IP>`.

   - **How to Debug:**
     - Print the IP address of the ESP32 to the Serial Monitor to make sure it's correct:
       ```cpp
       Serial.print("IP Address: ");
       Serial.println(WiFi.localIP());
       ```
     - Check if the web server code is running correctly. You can add a simple `Serial.println()` to confirm that the server has started:
       ```cpp
       server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
           Serial.println("Handling web request...");
           // Your code here
       });
       ```
     - Check if there are any issues with your router's firewall settings that might block access to the ESP32.

### 6. **General Debugging Tips**
   - **Problem:** General issues not covered above.
   - **Solution:**
     - Always check the Serial Monitor for error messages or debug output.
     - Add `Serial.println()` statements throughout the code to narrow down where the issue occurs.
     - Make sure your ESP32 board is selected correctly in the Arduino IDE (Tools > Board > ESP32 Dev Module).
     - Ensure the correct COM port is selected in the Arduino IDE (Tools > Port).

---

By following these troubleshooting and debugging steps, you should be able to identify and fix most issues with the ESP32 Weather Station project. If you encounter any other problems, feel free to raise an issue in the GitHub repository.
