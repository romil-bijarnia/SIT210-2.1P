#include <SPI.h>
#include <WiFiNINA.h>
#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

// WiFi credentials
const char* ssid     = "Romils Wifi";
const char* password = "romil2004";

// IFTTT settings
const char* host = "maker.ifttt.com";
const int httpPort = 80;
const char* iftttKey = "boP8mFZDiGwqlafGX5GBMd";  // Replace with your IFTTT key

// IFTTT events
const char* eventSunlightOn = "terrarium_light";
const char* eventSunlightOff = "terrarium_light";

// Sunlight detection threshold (lux)
const float luxThreshold = 400.0;  // Adjust this threshold based on testing
bool sunlightActive = false;

WiFiClient client;

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println("Arduino started...");

  Wire.begin();
  Serial.println("I2C (Wire) initialized...");

  if (lightMeter.begin()) {
    Serial.println("BH1750 initialized successfully.");
  } else {
    Serial.println("Error initializing BH1750. Check wiring.");
    while(1);
  }

  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    attempts++;
    if (attempts > 20) {  // Prevent infinite loop
      Serial.println("\nWiFi connection failed. Check credentials/router.");
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected successfully.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}


void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  if (lux > luxThreshold && !sunlightActive) {
    sunlightActive = true;
    sendIFTTTNotification(eventSunlightOn);
  } else if (lux <= luxThreshold && sunlightActive) {
    sunlightActive = false;
    sendIFTTTNotification(eventSunlightOff);
  }

  delay(2000);  // Check every 2 seconds
}

void sendIFTTTNotification(const char* eventName) {
  String url = "/trigger/";
  url += eventName;
  url += "/with/key/";
  url += iftttKey;

  if (client.connect(host, httpPort)) {
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    Serial.println("IFTTT notification sent.");
  } else {
    Serial.println("Connection to IFTTT failed.");
  }

  while(client.connected() && !client.available()) {
    delay(100);
  }
  while(client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  client.stop();
}
