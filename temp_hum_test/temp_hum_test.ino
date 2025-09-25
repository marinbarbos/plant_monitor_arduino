#include <DHT11.h>

const int PINO_DHT = 2;    // Pino que o sensor está conectado

DHT11 dht11(PINO_DHT);

const int NUMERO_AMOSTRAS = 100;

void setup() {
  Serial.begin(9600);
  Serial.println("Sensor de Umidade e Temperatura - Teste");
  Serial.println("Coletando media de leituras");
  int leitura_temperatura = 0;
  int leitura_umidade = 0;

  long somatoria_temperatura = 0;
  long somatoria_umidade = 0;

  for (int i = 1; i <= NUMERO_AMOSTRAS; i++) {
    int res = dht11.readTemperatureHumidity(leitura_temperatura, leitura_umidade);
    if (res == 0) {
      Serial.print("Temperatura: ");
      Serial.print(leitura_temperatura);
      somatoria_temperatura = somatoria_temperatura + leitura_temperatura; 
      Serial.print(" °C\tHumidity: ");
      Serial.print(leitura_umidade);
      somatoria_umidade = somatoria_umidade + leitura_umidade; 
      Serial.println(" %");
    } else {
      Serial.println(DHT11::getErrorString(res));
    }
    delay(1000);
  }

  int media_temperatura = somatoria_temperatura / NUMERO_AMOSTRAS;
  Serial.println("");
  Serial.print("Media Temp: ");
  Serial.println(media_temperatura);
  int media_umidade = somatoria_umidade / NUMERO_AMOSTRAS;
  Serial.println("");
  Serial.print("Media Umid: ");
  Serial.println(media_umidade);
}

void loop() {
  //TODO
}
