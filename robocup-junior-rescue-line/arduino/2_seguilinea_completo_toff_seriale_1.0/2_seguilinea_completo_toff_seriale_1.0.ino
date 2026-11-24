/***********************************************
  Codice per Arduino Due
  Seguilinea con sensori di linea analogici, 
  2 sensori ToF (VL53L0X) e controllo dei motori
  (Driver TB6612FNG controllato direttamente con 
   digitalWrite() e analogWrite())
***********************************************/

// DEFINIZIONI
#define Speed 100
#define LOX1_ADDRESS 0x30  // modifica se necessario
#define LOX2_ADDRESS 0x31  // modifica se necessario
#define SHT_LOX1 12
#define SHT_LOX2 13

// Pin per TB6612FNG
#define AIN1 7    // Motor1 input 1
#define AIN2 8    // Motor1 input 2
#define PWMA 10   // Motor1 PWM
#define BIN1 4    // Motor2 input 1
#define BIN2 5    // Motor2 input 2
#define PWMB 9    // Motor2 PWM
#define STBY 6    // Standby

// Soglie per i sensori di linea (da calibrare)
/*const int THRESHOLD_DIFF = 35;   // per i sensori laterali
const int THRESHOLD_DIFF_CENTER = 5;  // per il sensore centrale*/

// -------------------------
// LIBRERIE
// -------------------------
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

// -------------------------
// OGGETTI E VARIABILI PER I SENSORI TOF
// -------------------------
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;
int misura1 = 0;
int misura2 = 0;

// -------------------------
// VARIABILI PER I SENSORI DI LINEA (analogici)
// -------------------------
uint32_t s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0;
uint32_t s1_ = 0, s2_ = 0, s3_ = 0, s4_ = 0, s5_ = 0;
int Maxs = 0;
uint32_t ref_s1 = 0, ref_s2 = 0, ref_s3 = 0, ref_s4 = 0, ref_s5 = 0;

// -------------------------
// FLAG e VARIABILI DI CONTROLLO DEL SEGUILINEA
// -------------------------
bool a1 = false, a2 = false, a3 = false, a4 = false, a5 = false;
byte last_a3 = 0;
bool last_a2 = false, last_a4 = false;
int a = 0;
int sSpeed = 0;
int dSpeed = 0;
int biancodirezioneS = 1;
int biancodirezioneD = -1;

// -------------------------
// VARIABILI PER COMUNICAZIONE SERIAL
// -------------------------
String inputString = "";
boolean stringComplete = false;

// -------------------------
// TIMING
// -------------------------
uint32_t millisPausaMotore = 0;
uint32_t millisPausaToff = 0;

// -------------------------
// Funzioni per il controllo dei motori (usando i pin direttamente)
// -------------------------
void driveMotor1(int speed) {
  if (speed >= 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  }
  analogWrite(PWMA, abs(speed));
}

void driveMotor2(int speed) {
  if (speed >= 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  }
  analogWrite(PWMB, abs(speed));
}

// -------------------------
// Funzione setID() per i sensori ToF
// -------------------------
void setID() {
  digitalWrite(SHT_LOX1, LOW);    
  digitalWrite(SHT_LOX2, LOW);
  delay(10);
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);
  // Attiva LOX1 e resetta LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);
  if (!lox1.begin(LOX1_ADDRESS)) {
    //Serial.println(F("Failed to boot first VL53L0X"));
    while (1);
  }
  delay(10);
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);
  if (!lox2.begin(LOX2_ADDRESS)) {
    //Serial.println(F("Failed to boot second VL53L0X"));
    while (1);
  }
}

// -------------------------
// Funzione read_dual_sensors() per i sensori ToF
// -------------------------
void read_dual_sensors() {
  lox1.rangingTest(&measure1, false);
  lox2.rangingTest(&measure2, false);
  if (measure1.RangeStatus != 4) {
    misura1 = measure1.RangeMilliMeter;
  }
  if (measure2.RangeStatus != 4) {
    misura2 = measure2.RangeMilliMeter;
  }
}

// -------------------------
// Funzione bianco(): se nessun sensore di linea rileva la linea
// -------------------------
void bianco() {
   int j=0;
  if(a3==1){
    //Serial.print("fatto");
    return;
    
   }
  for(int i=0;i<=40;i++){
  driveMotor1(biancodirezioneD*100);
  driveMotor2(biancodirezioneS*100);
  delay(50);
  s1=analogRead(A1);
  s2=analogRead(A2);
  s3=analogRead(A3);
  s4=analogRead(A6);
  s5=analogRead(A7);
  s1_= ((s1 * (uint32_t)1000)/Maxs);
  s2_= ((s2 * (uint32_t)1000)/Maxs);
  s3_= ((s3 * (uint32_t)1000)/Maxs);
  s4_= ((s4 * (uint32_t)1000)/Maxs);
  s5_= ((s5 * (uint32_t)1000)/Maxs);
 if((s1_/2)<((ref_s1/2)-5)){a1=1; }else{a1=0;}
 if((s2_/2)<((ref_s2/2)-5)){a2=1; }else{a2=0;}
 if((s3_/2)<((ref_s3/2)-5)){a3=1; }else{a3=0;}
 if((s4_/2)<((ref_s4/2)-5)){a4=1; }else{a4=0;}
 if((s5_/2)<((ref_s5/2)-5)){a5=1; }else{a5=0;}

 if(a1==1 || a2==1 || a3==1 || a4==1 || a5==1){
 return;
 }
 else if(i==40){
 biancodirezioneD= -1*(biancodirezioneD);  
 biancodirezioneS= -1*(biancodirezioneS);
 driveMotor1(biancodirezioneD*100);
 driveMotor2(biancodirezioneS*100);
 delay(1800);
 i=0;
 j=j+1;
 }
 if (j==2){return;}
}
}

// -------------------------
// Gestione seriale
// -------------------------
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '&') {
      stringComplete = true;
    }
  }

}

void informazione() {
  Serial.println("---- INFO ----");
  Serial.print("Error: ");
  Serial.println(((-25 * a1) + (-5 * a2) + (1 * a3) + (5 * a4) + (25 * a5)));
  Serial.print("sSpeed: ");
  Serial.println(sSpeed);
  Serial.print("dSpeed: ");
  Serial.println(dSpeed);
  Serial.print("s1: "); Serial.print(s1); Serial.print("  ref: "); Serial.println(ref_s1);
  Serial.print("s2: "); Serial.print(s2); Serial.print("  ref: "); Serial.println(ref_s2);
  Serial.print("s3: "); Serial.print(s3); Serial.print("  ref: "); Serial.println(ref_s3);
  Serial.print("s4: "); Serial.print(s4); Serial.print("  ref: "); Serial.println(ref_s4);
  Serial.print("s5: "); Serial.print(s5); Serial.print("  ref: "); Serial.println(ref_s5);
  Serial.println("--------------");
}

// -------------------------
// SETUP PRINCIPALE
// -------------------------
void setup() {
  Serial.begin(9600);
  inputString.reserve(50);
  delay(500);
  
  // Configura i pin per i sensori ToF
  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);
  
  // Configura i pin per i motori
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);
  
  // Inizializza i sensori ToF
  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);
  delay(10);
  setID();
  
  // Calibrazione dei sensori di linea
  s1 = analogRead(A1);
  s2 = analogRead(A2);
  s3 = analogRead(A3);
  s4 = analogRead(A6);
  s5 = analogRead(A7);
  if(s1 > s2){Maxs = s1;} else{Maxs = s2;}
  if(Maxs > s3){Maxs = Maxs;} else{Maxs = s3;}
  if(Maxs > s4){Maxs = Maxs;} else{Maxs = s4;}
  if(Maxs > s5){Maxs = Maxs;} else{Maxs = s5;}
  
  ref_s1= ((s1 * (uint32_t)1000)/Maxs);
  ref_s2= ((s2 * (uint32_t)1000)/Maxs);
  ref_s3= ((s3 * (uint32_t)1000)/Maxs);
  ref_s4= ((s4 * (uint32_t)1000)/Maxs);
  ref_s5= ((s5 * (uint32_t)1000)/Maxs);
  Serial.println("Setup completato.");
}

// -------------------------
// LOOP PRINCIPALE
// -------------------------
void loop() {
  // Gestione pacchetto seriale
  if (stringComplete) {
     if (inputString[0] == 'V') {
        //raddrizzati
        s1 = analogRead(A1);
        s2 = analogRead(A2);
        s3 = analogRead(A3);
        s4 = analogRead(A6);
        s5 = analogRead(A7);
        
        s1_= ((s1 * (uint32_t)1000)/Maxs);
        s2_= ((s2 * (uint32_t)1000)/Maxs);
        s3_= ((s3 * (uint32_t)1000)/Maxs);
        s4_= ((s4 * (uint32_t)1000)/Maxs);
        s5_= ((s5 * (uint32_t)1000)/Maxs);
        
        if((s1_/2)<((ref_s1/2)-35)){a1=1; }else{a1=0;}
       if((s2_/2)<((ref_s2/2)-35)){a2=1; }else{a2=0;}
       if((s3_/2)<((ref_s3/2)-5)){a3=1; }else{a3=0;}
       if((s4_/2)<((ref_s4/2)-35)){a4=1; }else{a4=0;}
       if((s5_/2)<((ref_s5/2)-35)){a5=1; }else{a5=0;}
      
       if(a1==1 || a2 == 1 && a3==0){
        while(a3==0){
         driveMotor1(100);
         driveMotor2(-100);
         delay(80);
         s3 = analogRead(A3);
         s3_= ((s3 * (uint32_t)1000)/Maxs);
         if((s3_/2)<((ref_s3/2)-5)){a3=1; }else{a3=0;}
        }
       }
      
       if(a5==1 || a4 == 1 && a3==0){
        while(a3==0){
         driveMotor1(-100);
         driveMotor2(100);
         delay(80);
         s3 = analogRead(A3);
         s3_= ((s3 * (uint32_t)1000)/Maxs);
         if((s3_/2)<((ref_s3/2)-5)){a3=1; }else{a3=0;}
        }
       }
         driveMotor1(0);
         driveMotor2(0);
         delay(100);
         Serial.println('K');
      }

    if (inputString[0] == 'I') {
      driveMotor1(100);
      driveMotor2(-100);
      delay(3500);
      Serial.println('I');
      inputString[0] = '\0';
    }
    else if (inputString[0] == 'A') {
      Serial.println('A');
      inputString[0] = '\0';
    }
    else if (inputString[0] == 'S') {
      
      driveMotor1(-100);
      driveMotor2(100);
      delay(2500);
      Serial.println('S');
      inputString[0] = '\0';
    }
    else if (inputString[0] == 'D') {
      
      driveMotor1(100);
      driveMotor2(-100);
      delay(2500);
      Serial.println('D');
      inputString[0] = '\0';
    }
    else if (inputString[0] == 'f') {
      informazione();
    }
    inputString = "";
    stringComplete = false;  
  }
  
  // Lettura sensori di linea
  s1 = analogRead(A1);
  s2 = analogRead(A2);
  s3 = analogRead(A3);
  s4 = analogRead(A6);
  s5 = analogRead(A7);
  
  s1_= ((s1 * (uint32_t)1000)/Maxs);
  s2_= ((s2 * (uint32_t)1000)/Maxs);
  s3_= ((s3 * (uint32_t)1000)/Maxs);
  s4_= ((s4 * (uint32_t)1000)/Maxs);
  s5_= ((s5 * (uint32_t)1000)/Maxs);
  
  if((s1_/2)<((ref_s1/2)-35)){a1=1; }else{a1=0;}
 if((s2_/2)<((ref_s2/2)-35)){a2=1; }else{a2=0;}
 if((s3_/2)<((ref_s3/2)-5)){a3=1; }else{a3=0;}
 if((s4_/2)<((ref_s4/2)-35)){a4=1; }else{a4=0;}
 if((s5_/2)<((ref_s5/2)-35)){a5=1; }else{a5=0;}
  
  // Gestione ToF per correzione, ogni 100ms
  if((millis() - millisPausaToff) >= 100) {
    read_dual_sensors();
    if (misura1 < misura2) {
      if(misura1 < 100) {
        while(misura2 > misura1) {
          driveMotor1(80);
          driveMotor2(-80);
          delay(100);
          read_dual_sensors();
        }
      setID();
      }
      
    }
    if (misura2 < misura1) {
      if(misura2 < 100) {
        while(misura1 > misura2) {
          driveMotor1(-80);
          driveMotor2(80);
          delay(100);
          read_dual_sensors();
        }
      setID();
      }
      
    }
    millisPausaToff = millis();
  }
  
  // Routine "bianco": se nessun sensore rileva la linea
  while(a1==0 && a2==0 && a3==0 && a4==0 && a5==0){
  if(last_a3==1 && last_a4==0 && last_a2==0){
      a3 = true;
      last_a3 = 2;
    }
    driveMotor1(100);
    driveMotor2(100);
    delay(200);
    bianco();
  }
  
  // Calcolo dell'errore pesato
  a=((-25*a1)+(-5*a2)+(a3*1)+(a4*5)+(a5*25));
  
  if((a <= 4 && a >= -4) || a == 6) {
    sSpeed = Speed + a;
    dSpeed = Speed + a;
  }
  else if(a < -4) {
    sSpeed = Speed + (-a);
    dSpeed = -Speed + a;
  }
  else if(a > 4 && a != 6) {
    sSpeed = -Speed + (-a);
    dSpeed = Speed + a;
  }
  
  last_a4 = a4;
  last_a3 = a3;
  last_a2 = a2;
  
  if((millis() - millisPausaMotore) >= 100) {
    driveMotor1(dSpeed);
    driveMotor2(sSpeed);
    millisPausaMotore = millis();
  }
  //delay(10);
}
