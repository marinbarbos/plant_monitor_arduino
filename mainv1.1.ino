#include <SoftwareSerial.h>
//TODO make work
SoftwareSerial espSerial(2, 3);  // RX=2, TX=3

void setup() {
  Serial.begin(115200);
  espSerial.begin(115200);
  
  sendATCommand("AT", 2000);       
  sendATCommand("AT+CWMODE=2", 1000);      
  sendATCommand("AT+CWSAP=\"ArduinoAP\",\"12345678\",5,3", 2000); // Access point
  sendATCommand("AT+CIPMUX=1", 1000);            
  sendATCommand("AT+CIPSERVER=1,80", 1000);      
}

void loop() {
  if (espSerial.available()) {
    String response = espSerial.readString();
    Serial.print(response);
    
    if (response.indexOf("+IPD") != -1) {
      int sensorValue = analogRead(A0);
      int temp = sensorValue
      
      String jsonData = "{\"temperature\":" + String(temp) + 
                       ",\"humidity\":" + String(hum) + "}";
      String httpResponse = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: application/json\r\n"
                           "Connection: close\r\n\r\n" + 
                           jsonData;
      
      int connectionId = response.charAt(response.indexOf("+IPD,") + 5) - '0';
      
      String cipsend = "AT+CIPSEND=" + String(connectionId) + "," + 
                      String(httpResponse.length()) + "\r\n";
      sendATCommand(cipsend, 1000);
      sendATCommand(httpResponse, 1000);
      
      // Close connection
      sendATCommand("AT+CIPCLOSE=" + String(connectionId), 1000);
    }
  }
}

void sendATCommand(String command, int timeout) {
  Serial.print("Sending: ");
  Serial.println(command);
  espSerial.println(command);
  
  delay(timeout);
  while (espSerial.available()) {
    Serial.write(espSerial.read());
  }
}