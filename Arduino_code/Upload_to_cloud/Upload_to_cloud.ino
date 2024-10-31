#include <M5Core2.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <esp_wifi.h>        
#include <esp_wpa2.h> 
#include <TimeLib.h>
#include <SparkFun_ADS1219.h>
#include <ArduinoJson.h>

// The endpoint URL
const char* server = "biome-iot.uqcloud.net";
const int port = 443; // HTTPS port
WiFiClientSecure client;

// ADC object
SfeADS1219ArdI2C myADC;

// Wi-Fi credentials
const char* ssid = "UQ";
const char* password = ""; // Enter password
const char* username = ""; // Enter username

// NTP Server and time configuration
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0; // GMT offset in seconds
const int daylightOffset_sec = 36000; // 10 hours ahead in seconds

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, 60000);

unsigned long previousMillis = 0;
const long interval = 1000; // interval at which to print time (milliseconds)

void setup() {

  M5.begin();
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE);

  // Initialize Wi-Fi for WPA2-Enterprise
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t*)username, strlen(username));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t*)username, strlen(username));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t*)password, strlen(password));
  esp_wifi_sta_wpa2_ent_enable();

  WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.println("Connecting to WiFi...");
  }
  M5.Lcd.println("Connected to WiFi");

  // Initialize NTP Client
  timeClient.begin();
  timeClient.setTimeOffset(daylightOffset_sec);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.println("Initialized Time Client");

  delay(1000); 

  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  M5.Lcd.println("Initialized Serial");

  delay(100); 
  Wire.begin(); 

  // Initialize ADC
  while (myADC.begin() == false) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.println("ADC failed to begin. Please check your wiring!");
    Serial.println("ADC failed to begin. Please check your wiring! Retrying...");
    delay(1000);
  }

  Serial.println("ADC initialized");
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.println("ADC initialized");

  client.setInsecure();  // Bypass SSL certificate validation

  M5.Lcd.setTextSize(1);  // Smaller text size
}

// Function to read voltage from the ADC
float readVoltage() {
  myADC.startSync();
  while (myADC.dataReady() == false) {
    delay(10);
  }
  myADC.readConversion();
  return myADC.getConversionMillivolts(); // Return the voltage in millivolts
}

// Function to send data to the endpoint as a JSON array
void sendDataAsArray(const char* isoTime, float voltageP0N1, float voltageP2N3) {
  if (client.connect(server, port)) {
    //Serial.println("Connected to server");

    // Create a JSON array
    DynamicJsonDocument doc(256);
    JsonArray dataArray = doc.to<JsonArray>();

    // Add sensor 1 data
    JsonObject record1 = dataArray.createNestedObject();
    record1["timestamp"] = isoTime;
    record1["sensor_id"] = 1;
    record1["sensor_name"] = "voltage vs. time";
    record1["sensor_value"] = voltageP0N1;

    // Add sensor 2 data
    JsonObject record2 = dataArray.createNestedObject();
    record2["timestamp"] = isoTime;
    record2["sensor_id"] = 2;
    record2["sensor_name"] = "voltage vs. time";
    record2["sensor_value"] = voltageP2N3;

    // Serialize the array to JSON string
    String jsonString;
    serializeJson(doc, jsonString);

    // Print the JSON array for debugging
    // Serial.println("JSON Payload:");
    // Serial.println(jsonString);

    // Send the POST request with the JSON array
    client.println("POST /api/monitoring_data HTTP/1.1");
    client.println("Host: biome-iot.uqcloud.net");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(jsonString.length());
    client.println();
    client.println(jsonString);

    // Print server response for debugging
    // while (client.connected()) {
    //   String response = client.readString();
    //   if (response.length() > 0) {
    //     Serial.println("Response from server:");
    //     Serial.println(response);
    //     break;
    //   }
    // }

    client.stop();
  } else {
    Serial.println("Connection to server failed");
    M5.Lcd.fillScreen(BLACK); // Clear the screen
    M5.Lcd.setCursor(10, 30);
    M5.Lcd.printf("Connection to server failed");
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;

    M5.Lcd.fillScreen(BLACK); // Clear the screen
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();

    char isoTime[25];
    sprintf(isoTime, "%04d-%02d-%02dT%02d:%02d:%02dZ", 
            year(epochTime), month(epochTime), day(epochTime), 
            hour(epochTime), minute(epochTime), second(epochTime));

    // Read differential voltage P0-N1
    myADC.setInputMultiplexer(ADS1219_CONFIG_MUX_DIFF_P0_N1);
    float voltageP0N1 = readVoltage();

    // Read differential voltage P2-N3
    myADC.setInputMultiplexer(ADS1219_CONFIG_MUX_DIFF_P2_N3);
    float voltageP2N3 = readVoltage();

    // Display Sensor 1 reading on the screen
    M5.Lcd.setCursor(10, 30);
    M5.Lcd.printf("Sensor 1 (P0-N1): %.3f mV", voltageP0N1);

    // Display Sensor 2 reading on the screen
    M5.Lcd.setCursor(10, 60);
    M5.Lcd.printf("Sensor 2 (P2-N3): %.3f mV", voltageP2N3);

    // Display Sensor 2 reading on the screen
    M5.Lcd.setCursor(10, 90);
    M5.Lcd.printf("Time: %s", isoTime);

    // Send both readings in one JSON array
    sendDataAsArray(isoTime, voltageP0N1, voltageP2N3);
  }
}