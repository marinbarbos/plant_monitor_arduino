/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-web-server
 */

#include <WiFi.h>
#include <DHT.h>
#include <ESPAsyncWebServer.h>

#define SOIL_PIN 36
#define DHT_PIN 33
#define LIGHT_PIN 34

const char* ssid = "";              // CHANGE IT
const char* password = "";  // CHANGE IT

AsyncWebServer server(80);
DHT dht11(DHT_PIN, DHT11);

void setup() {
  Serial.begin(9600);

  // set the ADC attenuation to 11 dB (up to ~3.3V input)
  analogSetAttenuation(ADC_11db);

  dht11.begin();  // initialize the DHT11 sensor
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print the ESP32's IP address
  Serial.print("ESP32 Web Server's IP address: ");
  Serial.println(WiFi.localIP());

  // Define a route to serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("ESP32 Web Server: New request received:");  // for debugging
    Serial.println("GET /");                                    // for debugging
    request->send(200, "text/html", "<html><body><h1>Hello, ESP32!</h1></body></html>");
  });

  // Start the server
  server.begin();
}

void loop() {
  int soilAnalogValue = analogRead(SOIL_PIN);  // read the analog value from sensor

  Serial.print("Moisture value: ");
  Serial.println(soilAnalogValue);

  // read humidity
  float hum = dht11.readHumidity();
  // read temperature in Celsius
  float temp = dht11.readTemperature();

  // check whether the reading is successful or not
  if (isnan(temp)|| isnan(hum)) {
    Serial.println("Failed to read from DHT11 sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print("%");

    Serial.print("  |  ");

    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print("°C ");
  }

  // reads the input on analog pin (value between 0 and 4095)
  int lightAnalogValue = analogRead(LIGHT_PIN);

  Serial.print("Analog Value = ");
  Serial.print(lightAnalogValue);  // the raw analog reading

  // We'll have a few threshholds, qualitatively determined
  if (lightAnalogValue < 40) {
    Serial.println(" => Dark");
  } else if (lightAnalogValue < 800) {
    Serial.println(" => Dim");
  } else if (lightAnalogValue < 2000) {
    Serial.println(" => Light");
  } else if (lightAnalogValue < 3200) {
    Serial.println(" => Bright");
  } else {
    Serial.println(" => Very bright");
  }


  delay(500);
  //String jsonData = "{\"temperature\":" + String(temp) + ",\"humidity\":" + String(hum) + "}";
}
