const int PINO_SENSOR = A1; // Pino que o sensor esta conectado

const int NUMERO_AMOSTRAS = 100;


void setup() {
  Serial.begin(9600);
  Serial.println("Sensor de Umidade de Solo Capacitivo - Primeiros Passos");
  Serial.println("Coletando media de leituras");
  
  int leitura_sensor = 0;
  
  long somatoria = 0;
  
  pinMode(PINO_SENSOR, INPUT);

  for (int i = 1; i <= NUMERO_AMOSTRAS; i++) {
    leitura_sensor = analogRead(PINO_SENSOR);
    somatoria = somatoria + leitura_sensor;        
    float tensao = leitura_sensor * (5.0 / 1023);
    Serial.print("Amostra ");
    Serial.print(i);
    Serial.print("  |  ");
    Serial.print("Leitura: ");
    Serial.print(leitura_sensor);
    Serial.print("  |  ");
    Serial.print("Tensao: ");
    Serial.println(tensao);
    delay(1000);
  }

  int media = somatoria / NUMERO_AMOSTRAS;
  Serial.println("");
  Serial.print("Media obtida: ");
  Serial.println(media);
}

void loop() {
  // TODO
}