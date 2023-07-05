#include <WiFi.h>
#include <Wire.h>
#include <BH1750.h>
#include <ThingsBoard.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>

const char* ssid = "Cozy binus 217";
const char* password = "12345678";

// ThingsBoard credentials and settings
const char* thing_server = "demo.thingsboard.io";
const char* thing_token = "saKLlzRuwIOlk5lvJIiD";

// Pin definitions
const int LIGHT_SENSOR_ADDRESS = 0x23; // I2C address of light sensor

const int DHT_PIN = 5; // DHT22 data pin

// Sensor type definitions
const int LIGHT_SENSOR_TYPE = BH1750::ONE_TIME_HIGH_RES_MODE; // 1 lx resolution
const int DHT_TYPE = DHT22;

// Sensor objects
BH1750 lightMeter; // I2C address 0x23
DHT dht(DHT_PIN, DHT_TYPE);

WiFiClient wifiClient;
ThingsBoard thing(wifiClient);

void WifiConnect(){

}

void setup() {
  Serial.begin(115200); // Initialize serial communication
  Wire.begin(21,22); // Initialize I2C
  
  lightMeter.begin(); // Initialize light sensor
  dht.begin(); // Initialize DHT22 sensor
  
  delay(100);  // Wait for the sensors to stabilize

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");

  // Connect to ThingsBoard
    if (!thing.connect(thing_server, thing_token)) {
    Serial.println("Failed to connect to ThingsBoard");
    }


}

void loop() {
  static unsigned long lastLightTime = 0; // data dikirim setiap 2 detik

  if (millis() - lastLightTime >= 2000) {
    uint16_t lux = lightMeter.readLightLevel(); // untuk light sensor
    float temperature = dht.readTemperature(); // DHT22
    float humidity = dht.readHumidity(); 
    
    Serial.print("Light Level: ");
    Serial.print(lux);
    Serial.println(" lux");

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // mengirim data ke thingsboard
    thing.sendTelemetryFloat("lightLevel", lux);
    thing.sendTelemetryFloat("temperature", temperature);
    thing.sendTelemetryFloat("humidity", humidity);

    lastLightTime = millis();
  }

  // supaya bisa thingboard terus menerus
  thing.loop();
}