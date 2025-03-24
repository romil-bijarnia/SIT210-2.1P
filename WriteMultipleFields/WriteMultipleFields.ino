/*
  DHT + ThingSpeak (WriteMultipleFields) for Arduino Nano 33 IoT

  - Reads temperature & humidity from a DHT sensor.
  - Sends these values to a ThingSpeak channel every 60 seconds.
  - Uses WiFiNINA for Wi-Fi connectivity.

  Requirements:
    1) WiFiNINA library
    2) ThingSpeak library
    3) DHT sensor library
    4) secrets.h file with your Wi-Fi and ThingSpeak details

  Steps to Complete the Task:
    - Wire the DHT sensor (VCC to 3.3V, GND to GND, DATA to D2 or another digital pin).
    - Create a ThingSpeak channel with at least two fields (Field 1 = Temperature, Field 2 = Humidity).
    - Update secrets.h with your Wi-Fi credentials, Channel ID, and Write API Key.
    - Upload this code. Every 60 seconds, your Arduino will send real sensor data to ThingSpeak.
*/

#include <WiFiNINA.h>
#include <ThingSpeak.h>
#include <DHT.h>
#include "secrets.h" // Contains SECRET_SSID, SECRET_PASS, SECRET_CH_ID, SECRET_WRITE_APIKEY

// ------------------- Wi-Fi Credentials -------------------
char ssid[] = "Romils Wifi";   // e.g., "Romils Wifi"
char pass[] = "romil2004";   // e.g., "romil2004"

// ------------------- ThingSpeak Channel Info -------------------
unsigned long myChannelNumber = SECRET_CH_ID;        // e.g., 2890350
const char * myWriteAPIKey   = "SVBXDATHHDB4WDY7";  // e.g., "SXBtAHDTH8bWHD17"

// ------------------- DHT Sensor Setup -------------------
// If you wired DATA to pin 2, set DHTPIN to 2. 
// If you're using a DHT11, set DHTTYPE to DHT11; for DHT22, set DHTTYPE to DHT22.
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Create a WiFi client object
WiFiClient client;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for Serial Monitor
  }

  // Initialize the DHT sensor
  dht.begin();

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  Serial.println("Setup complete. Will connect to Wi-Fi in loop().");
}

void loop() {
  // 1. Check Wi-Fi connection; reconnect if necessary
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected to Wi-Fi.");
  }

  // 2. Read the temperature and humidity from DHT
  float temperature = dht.readTemperature(); // Celsius by default
  float humidity    = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    // Print the readings to Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // 3. Set fields for ThingSpeak
    ThingSpeak.setField(1, temperature); // Field 1 = Temperature
    ThingSpeak.setField(2, humidity);    // Field 2 = Humidity

    // 4. Send data to ThingSpeak
    int httpResponse = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (httpResponse == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.print("Problem updating channel. HTTP error code: ");
      Serial.println(httpResponse);
    }
  }

  // 5. Wait 0.6 seconds before next update (task requirement)
  delay(6);
}
