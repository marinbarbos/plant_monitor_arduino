/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>
#include <DHT.h>

#define SOIL_PIN 36
#define DHT_PIN 33
#define LIGHT_PIN 35

// Replace with your network credentials
const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);
DHT dht11(DHT_PIN, DHT11);
// Variable to store the HTTP request
String header;

void setup() {
  Serial.begin(9600);
  // set the ADC attenuation to 11 dB (up to ~3.3V input)
  analogSetAttenuation(ADC_11db);
  // Initialize the output variables as outputs
  dht11.begin();  // initialize the DHT11 sensor

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();
}

void loop() {
  WiFiClient client = server.available();  // Listen for incoming clients

  if (client) {                     // If a new client connects,
    Serial.println("New Client.");  // print a message out in the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        header += c;
        if (c == '\n') {  // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println("Connection: close");
            client.println();
            
            int soilAnalogValue = analogRead(SOIL_PIN);  // read the analog value from sensor

            Serial.print("Moisture value: ");
            Serial.println(soilAnalogValue);

            // read humidity
            float hum = dht11.readHumidity();
            // read temperature in Celsius
            float temp = dht11.readTemperature();

            // check whether the reading is successful or not
            if (isnan(temp) || isnan(hum)) {
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

            String jsonData = "{\"airTemperature\":" + String(temp) + ",\"airHumidity\":" + String(hum) + ",\"luminosity\":" + String(lightAnalogValue) + ",\"soilHumidity\":" + String(soilAnalogValue) + "}";

            // Display the HTML web page
            client.println(jsonData);
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else {  // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
