const int PINO_SENSOR = A0;  // Pino que o sensor está conectado

const int NUMERO_AMOSTRAS = 100;

void setup() {
  Serial.begin(9600);

  Serial.println("Sensor de Luminosidade - Teste");
  Serial.println("Coletando media de leituras");

  int leitura_sensor = 0;
  
  long somatoria = 0;

  pinMode(PINO_SENSOR, INPUT);

  for (int i = 1; i <= NUMERO_AMOSTRAS; i++) {
    leitura_sensor = analogRead(PINO_SENSOR);
    somatoria = somatoria + leitura_sensor;
    Serial.print("Amostra ");
    Serial.print(i);
    Serial.print("  |  ");
    Serial.print("Luz = "); 
    Serial.println(leitura_sensor);
    delay(1000);
  }

  int media = somatoria / NUMERO_AMOSTRAS;
  Serial.println("");
  Serial.print("Media obtida: ");
  Serial.println(media);
}

void loop() {
  //TODO
}
