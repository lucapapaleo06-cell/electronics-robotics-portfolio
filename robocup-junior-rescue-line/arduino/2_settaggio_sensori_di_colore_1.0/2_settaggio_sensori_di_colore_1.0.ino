// Definizioni per il primo sensore TCS230/TCS3200
#define S1_S0 22   // Controllo scala frequenza
#define S1_S1 24   // Controllo scala frequenza
#define S1_S2 26   // Selezione filtro: per verde → HIGH
#define S1_S3 28   // Selezione filtro: per verde → LOW
#define S1_OUT 30  // Uscita del sensore
//#define Led_1 32

// Definizioni per il secondo sensore TCS230/TCS3200
#define S2_S0 42   // Controllo scala frequenza
#define S2_S1 44   // Controllo scala frequenza
#define S2_S2 46  // Selezione filtro: per verde → HIGH
#define S2_S3 48   // Selezione filtro: per verde → LOW
#define S2_OUT 50  // Uscita del sensore
//#define Led_2 52

// Impostazioni: 100% scaling per entrambi i sensori
// (S0 = HIGH, S1 = LOW per 100% scaling)
// Per selezionare il filtro verde, impostiamo S2 = HIGH e S3 = LOW

void setup() {
  Serial.begin(9600);
 /* pinMode(Led_1, OUTPUT);
  pinMode(Led_2, OUTPUT);
  digitalWrite(Led_1, HIGH);
  digitalWrite(Led_2, LOW);*/
  // Configura i pin di controllo per il sensore 1 come OUTPUT
  pinMode(S1_S0, OUTPUT);
  pinMode(S1_S1, OUTPUT);
  pinMode(S1_S2, OUTPUT);
  pinMode(S1_S3, OUTPUT);
  // Imposta i valori: 100% scaling e filtro verde
  digitalWrite(S1_S0, HIGH);
  digitalWrite(S1_S1, LOW);
  digitalWrite(S1_S2, HIGH);
  digitalWrite(S1_S3, LOW);
  // Imposta il pin di uscita come INPUT
  pinMode(S1_OUT, INPUT);
  
  // Configura i pin di controllo per il sensore 2 come OUTPUT
  pinMode(S2_S0, OUTPUT);
  pinMode(S2_S1, OUTPUT);
  pinMode(S2_S2, OUTPUT);
  pinMode(S2_S3, OUTPUT);
  // Imposta i valori: 100% scaling e filtro verde
  digitalWrite(S2_S0, HIGH);
  digitalWrite(S2_S1, LOW);
  digitalWrite(S2_S2, HIGH);
  digitalWrite(S2_S3, LOW);
  // Imposta il pin di uscita come INPUT
  pinMode(S2_OUT, INPUT);
  
  Serial.println("Setup completato. Inizio lettura dei sensori di colore per rilevare il verde.");
}

void loop() {
  // Misura il periodo (in microsecondi) per il sensore 1
  unsigned long period1 = pulseIn(S1_OUT, LOW, 1000000);  // Timeout 1 sec
  // Misura il periodo per il sensore 2
  unsigned long period2 = pulseIn(S2_OUT, LOW, 1000000);
  
  // Converti il periodo in frequenza (Hz)
  // Se period è 0, la frequenza viene impostata a 0
  float freq1 = (period1 > 0) ? 1000000.0 / period1 : 0;
  float freq2 = (period2 > 0) ? 1000000.0 / period2 : 0;
  
  // Stampa i valori letti
  Serial.print("Sensor1 Frequency: ");
  Serial.print(freq1);
  Serial.print(" Hz\t");
  Serial.print("Sensor2 Frequency: ");
  Serial.print(freq2);
  Serial.println(" Hz");
  
  // Puoi usare questi valori per calibrare le soglie per rilevare quadratini verdi.
  // Ad esempio, osserva i valori stampati quando posizioni un quadratino verde davanti al sensore.
  
  delay(500);
}
