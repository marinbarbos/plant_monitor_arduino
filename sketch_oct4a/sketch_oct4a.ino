/*********
  Plant Monitor ESP32 - Micro Garden Project
  Provides API endpoints for Flutter app integration
*********/

#include <WiFi.h>
#include <DHT.h>

#define SOIL_PIN 36
#define DHT_PIN 33
#define LIGHT_PIN 35

// DHT11 Sensor
DHT dht11(DHT_PIN, DHT11);

// Network Credentials
const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

// Web Server
WiFiServer server(80);

// Sensor calibration values (adjust based on your sensors)
const int DRY_SOIL = 4095;    // Soil moisture when completely dry
const int WET_SOIL = 1500;    // Soil moisture when completely wet

// I2C Pins (ESP32 default)
//#define I2C_SDA 21
//#define I2C_SCL 22

// LCD Setup - Most common I2C address is 0x27, but could be 0x3F
//LiquidCrystal_I2C lcd(0x27, 16, 2);  // Address 0x27, 16 columns, 2 rows

void setup() {
  delay(2000);

  Serial.begin(115200);
  // Wait for serial to be ready
  while(!Serial && millis() < 5000);
  // Set ADC attenuation
  analogSetAttenuation(ADC_11db);
    yield();
  // Initialize I2C
  //Wire.begin(I2C_SDA, I2C_SCL);
  
  // Initialize LCD
  //lcd.init();
  //lcd.backlight();
  //lcd.clear();
  
  // Show startup message
  //lcd.setCursor(0, 0);
  //lcd.print("Micro Garden");
  //lcd.setCursor(0, 1);
  //lcd.print("Starting...");
  //delay(2000);

  // Initialize DHT11 sensor
  // dht11.begin();
  
  // Setup Access Point
  //lcd.clear();
  //lcd.setCursor(0, 0);
  //lcd.print("Setting up AP...");
  //Serial.println("Setting up Access Point...");

  // Setup Access Point
  Serial.println("Setting up Access Point...");
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  // Display IP on LCD
  //lcd.clear();
  //lcd.setCursor(0, 0);
  //lcd.print("IP Address:");
  //lcd.setCursor(0, 1);
  //lcd.print(IP);
  
  server.begin();
  Serial.println("Server started");
}

void loop() {
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New Client Connected");
    String currentLine = "";
    String request = "";
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Parse the request
            handleRequest(client, request);
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    
    client.stop();
    Serial.println("Client Disconnected\n");
  }
}

void handleRequest(WiFiClient &client, String request) {
  // Check which endpoint was requested
  if (request.indexOf("GET /api/health") >= 0) {
    sendHealthResponse(client);
  } else if (request.indexOf("GET /api/status") >= 0) {
    sendStatusResponse(client);
  } else {
    send404Response(client);
  }
}

void sendHealthResponse(WiFiClient &client) {
  // Simple health check endpoint
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Connection: close");
  client.println();
  client.println("{\"status\":\"ok\",\"uptime\":" + String(millis()) + "}");
  
  Serial.println("Health check requested - OK");
}

void sendStatusResponse(WiFiClient &client) {
  // Read all sensors
  float temperature = dht11.readTemperature();
  float humidity = dht11.readHumidity();
  int lightRaw = analogRead(LIGHT_PIN);
  int soilRaw = analogRead(SOIL_PIN);
  
  // Convert soil moisture to percentage (0-100%)
  float soilMoisture = map(soilRaw, DRY_SOIL, WET_SOIL, 0, 100);
  soilMoisture = constrain(soilMoisture, 0, 100);
  
  // Check for sensor errors
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT11 sensor read error!");
    temperature = 0.0;
    humidity = 0.0;
  }
  
  // Log readings
  Serial.println("=== Sensor Readings ===");
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" °C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
  Serial.print("Light (raw): "); Serial.println(lightRaw);
  Serial.print("Soil Moisture: "); Serial.print(soilMoisture); Serial.println(" %");
  
  // Get recommendations
  String tempRec = getTemperatureRecommendation(temperature);
  String lightRec = getLightRecommendation(lightRaw);
  String soilRec = getSoilMoistureRecommendation(soilMoisture);
  
  // Get current timestamp (milliseconds since boot)
  unsigned long timestamp = millis();
  
  // Build JSON response
  String json = "{";
  json += "\"temperature\":" + String(temperature, 1) + ",";
  json += "\"humidity\":" + String(humidity, 1) + ",";
  json += "\"light\":" + String(lightRaw) + ",";
  json += "\"soil_moisture\":" + String(soilMoisture, 1) + ",";
  json += "\"timestamp\":\"" + getISOTimestamp(timestamp) + "\",";
  json += "\"status\":\"ok\",";
  json += "\"recommendations\":{";
  json += "\"temperature\":\"" + tempRec + "\",";
  json += "\"light\":\"" + lightRec + "\",";
  json += "\"soil_moisture\":\"" + soilRec + "\"";
  json += "}";
  json += "}";
  
  // Send HTTP response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Connection: close");
  client.println("Content-Length: " + String(json.length()));
  client.println();
  client.println(json);
  
  Serial.println("Status data sent successfully");
  Serial.println("========================\n");
}

void send404Response(WiFiClient &client) {
  client.println("HTTP/1.1 404 Not Found");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();
  client.println("{\"error\":\"Endpoint not found\"}");
  
  Serial.println("404 - Unknown endpoint requested");
}

// Recommendation functions based on Flutter app logic
String getTemperatureRecommendation(float temp) {
  if (temp >= 20.0 && temp <= 25.0) {
    return "ideal";
  } else if (temp < 20.0) {
    return "aumentar";
  } else {
    return "diminuir";
  }
}

String getLightRecommendation(int light) {
  if (light >= 400 && light <= 2000) {
    return "ideal";
  } else if (light < 400) {
    return "aumentar";
  } else {
    return "diminuir";
  }
}

String getSoilMoistureRecommendation(float moisture) {
  if (moisture >= 50.0 && moisture <= 75.0) {
    return "ideal";
  } else if (moisture < 50.0) {
    return "aumentar";
  } else {
    return "diminuir";
  }
}

// Generate ISO 8601 timestamp string
String getISOTimestamp(unsigned long millis) {
  // Since ESP32 doesn't have RTC by default, we'll use a simple format
  // In production, you'd want to use NTP or an RTC module
  unsigned long seconds = millis / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  
  seconds = seconds % 60;
  minutes = minutes % 60;
  hours = hours % 24;
  
  char timestamp[30];
  sprintf(timestamp, "xxxx-yy-zzT%02lu:%02lu:%02lu.000Z", hours, minutes, seconds);
  return String(timestamp);
}