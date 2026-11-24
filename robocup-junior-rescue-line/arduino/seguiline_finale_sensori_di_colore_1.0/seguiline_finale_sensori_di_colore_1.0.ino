#define Speed 100
#include <SparkFun_TB6612.h>
#define AIN1 45  //Assign the motor pins
#define BIN1 51
#define AIN2 47
#define BIN2 53
#define PWMA 2
#define PWMB 3
#define STBY 49
//destra e sinistra sono invertiti
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
float Freq1 = 7000.0;  // Valore che prendo dal settaggio dei sensori esempio in Hz
float Freq2 = 7000.0;  // Valore che prendo dal settaggio dei sensori esempio in Hz
byte verde=0;
uint32_t s1=0;
uint32_t s2=0;
uint32_t s3=0;
uint32_t s4=0;
uint32_t s5=0;

uint32_t s1_=0;
uint32_t s2_=0;
uint32_t s3_=0;
uint32_t s4_=0;
uint32_t s5_=0;

int Maxs=0;

uint32_t ref_s1=0;
uint32_t ref_s2=0;
uint32_t ref_s3=0;
uint32_t ref_s4=0;
uint32_t ref_s5=0;

bool a1=0;
bool a2=0;
bool a3=0;
bool a4=0;
bool a5=0;
byte last_a3=0;
bool last_a2=0;
bool last_a4=0;

int a=0;
int sSpeed = 0;
int dSpeed = 0;
int biancodirezioneS=1;
int biancodirezioneD=-1;


const int offsetA = 1;
const int offsetB = 1;

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

String inputString = "";  
boolean  stringComplete = false;

uint32_t millisPausaMotore;

void setup() {
  Serial.begin(9600);
    // reserve 50 bytes for the inputString:
  inputString.reserve(50);
   while(inputString[0] != 'R'){
  serialEvent();
  }
  if (inputString[0] == 'R'){
  Serial.print("R&");
  inputString = "";
  stringComplete = false;
}

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
  

  delay(500);
  pinMode(AIN2, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  digitalWrite(STBY, HIGH);
  s1=analogRead(A1);
  s2=analogRead(A2);
  s3=analogRead(A3);
  s4=analogRead(A4);
  s5=analogRead(A5);


  if(s1 > s2){Maxs = s1;} else{Maxs = s2;}
  if(Maxs > s3){Maxs = Maxs;} else{Maxs = s3;}
  if(Maxs > s4){Maxs = Maxs;} else{Maxs = s4;}
  if(Maxs > s5){Maxs = Maxs;} else{Maxs = s5;}

  ref_s1= ((s1 * (uint32_t)1000)/Maxs);
  ref_s2= ((s2 * (uint32_t)1000)/Maxs);
  ref_s3= ((s3 * (uint32_t)1000)/Maxs);
  ref_s4= ((s4 * (uint32_t)1000)/Maxs);
  ref_s5= ((s5 * (uint32_t)1000)/Maxs);

}

void loop() {
  // PACCHETTO SERIALE
  //***--------------
   if (stringComplete)
   { if (inputString[0] == 'V') {
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
         motor1.drive(100);
         motor2.drive(-100);
         delay(80);
         s3 = analogRead(A3);
         s3_= ((s3 * (uint32_t)1000)/Maxs);
         if((s3_/2)<((ref_s3/2)-5)){a3=1; }else{a3=0;}
        }
       }
      
       if(a5==1 || a4 == 1 && a3==0){
        while(a3==0){
         motor1.drive(-100);
         motor2.drive(100);
         delay(80);
         s3 = analogRead(A3);
         s3_= ((s3 * (uint32_t)1000)/Maxs);
         if((s3_/2)<((ref_s3/2)-5)){a3=1; }else{a3=0;}
        }
       }
         motor1.drive(0);
         motor2.drive(0);
         delay(100);
         Serial.println("K&");
      }
      else if(inputString[0] == 'I'){
       motor1.drive(100);
       motor2.drive(-100);
       delay(4000);
       Serial.println("I&");
       inputString[0] = '\0';
      
       }
      else if(inputString[0] == 'A'){Serial.println('A');
      inputString[0] = '\0';
      }
      
      else if(inputString[0] == 'S'){
      motor1.drive(100);
      motor2.drive(100);
      delay(1500);
      motor1.drive(100);
       motor2.drive(-100);
       delay(2400);
       Serial.println("S&");
     inputString[0] = '\0';
      
    }
       
      else if(inputString[0] == 'D'){
      motor1.drive(100);
      motor2.drive(100);
      delay(1500);
       motor1.drive(-100);
       motor2.drive(100);
       delay(2400);
       Serial.println("D&");
      inputString[0] = '\0';
       }
       
      else if(inputString[0] == 'f'){informazione();}
    
    // clear the string:
    inputString = "";
    stringComplete = false;   
   }

 /* unsigned long period1 = pulseIn(S1_OUT, LOW, 1000000);  // Timeout 1 sec
  // Misura il periodo per il sensore 2
  unsigned long period2 = pulseIn(S2_OUT, LOW, 1000000);
  // Converti il periodo in frequenza (Hz)
  // Se period è 0, la frequenza viene impostata a 0
  float freq1 = (period1 > 0) ? 1000000.0 / period1 : 0;
  float freq2 = (period2 > 0) ? 1000000.0 / period2 : 0;*/
   //***=============  
  s1=analogRead(A1);
  s2=analogRead(A2);
  s3=analogRead(A3);
  s4=analogRead(A4);
  s5=analogRead(A5);

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


////Giroscopio/////
/*
 if ((freq1 > Freq1) || (freq2 > Freq2)) {
    if(a2==1 && a3==1 && a4==1 || a5==1 || a1==1){
    //motor1.drive(0);
    //motor2.drive(0);
    if(freq1 > Freq1){
      if(freq2 > Freq2){
        verde=3;
      }else{ verde=1;}
    }
    if(freq2 > Freq2){
      if(freq1 > Freq1){
        verde=3;
      }else{ verde=2;}
   }
   verdi();
   }}*/

 ///comendo bianco///
while(a1==0 && a2==0 && a3==0 && a4==0 && a5==0){
  /*if(last_a3==1 && last_a4==0 && last_a2==0){
    a3=1;
    last_a3=2;
  }  */
  motor1.drive(100);
  motor2.drive(100);
  delay(100);
  bianco();

}



 
 
////comando normale///
a=((-25*a1)+(-5*a2)+(a3*1)+(a4*5)+(a5*25));

if((a<=4 && a>=-4) || a==6){
  sSpeed=Speed+(a);
  dSpeed=Speed+(a);
 }
 else if(a<-4){
  
  sSpeed=Speed+(-a);
  dSpeed=-Speed+(a);
 }
  else if(a>4 && a!=6){
  
  sSpeed=-Speed+(-a);
  dSpeed=Speed+(a);
 }

/*last_a4=a4;
last_a3=a3;
last_a2=a2;
*/
  if((millis() - millisPausaMotore) >= 100)
  {
    //informazione();
    motor1.drive(sSpeed);
    motor2.drive(dSpeed);

    millisPausaMotore = millis();
  }

}
/*void verdi() {
  
switch (verde) {
    case 1://sinistra
      motor1.drive(100);
      motor2.drive(-100);
      delay(2500);
      verde=0;
      break;
    case 2://destra
      motor1.drive(100);
      motor2.drive(-100);
      delay(2500);
      verde=0;
      break;
    case 3://indietro
      motor1.drive(100);
      motor2.drive(-100);
      delay(3500);
      verde=0;
      break;
  }

}*/
void bianco(){
  int j=0;
 /* if(a3==1){
    Serial.print("fatto");
    return;
    
   }*/
  for(int i=0;i<=40;i++){
  motor1.drive(biancodirezioneD*100);
  motor2.drive(biancodirezioneS*100);
  delay(50);
  s1=analogRead(A1);
  s2=analogRead(A2);
  s3=analogRead(A3);
  s4=analogRead(A4);
  s5=analogRead(A5);
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

 if(a1==1 || a2==1 || a3==1 || a4==1 || a5==1){
 return loop();
 }
 else if(i==40){
 biancodirezioneD= -1*(biancodirezioneD);  
 biancodirezioneS= -1*(biancodirezioneS);
 motor1.drive(biancodirezioneD*100);
 motor2.drive(biancodirezioneS*100);
 delay(1800);
 i=0;
 j=j+1;
 }
 if (j==2){return loop();}
}
}

void serialEvent() 
{
  while (Serial.available()) 
  {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '&') {stringComplete = true;}
  }
}
void informazione()
{
  Serial.println(Maxs);
  Serial.println(a);
  Serial.print("sinistra= ");Serial.println(sSpeed);
  Serial.print("destra= "); Serial.println(dSpeed);
  Serial.print("1= "); Serial.print((ref_s1)); Serial.print("  "); Serial.print(a1); Serial.print("  "); Serial.println(s1);
  Serial.print("2= "); Serial.print((ref_s2)); Serial.print("  "); Serial.print(a2); Serial.print("  "); Serial.println(s2);
  Serial.print("3= "); Serial.print((ref_s3)); Serial.print("  "); Serial.print(a3); Serial.print("  "); Serial.println(s3);
  Serial.print("4= "); Serial.print((ref_s4)); Serial.print("  "); Serial.print(a4); Serial.print("  "); Serial.println(s4);
  Serial.print("5= "); Serial.print((ref_s5));  Serial.print("  ");Serial.print(a5); Serial.print("  "); Serial.println(s5);
  Serial.println( );
}
