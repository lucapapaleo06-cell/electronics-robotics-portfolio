#define Speed 100
#include <SparkFun_TB6612.h>//https://github.com/sparkfun/SparkFun_TB6612FNG_Arduino_Library.git
#define AIN1 7  //Assign the motor pins
#define BIN1 4
#define AIN2 8
#define BIN2 5
#define PWMA 10
#define PWMB 9
#define STBY 6
//destra e sinistra sono invertiti
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

void setup()
{ 
  
 
  Serial.begin(9600);
    // reserve 50 bytes for the inputString:
  inputString.reserve(50);
  while(inputString[0] != 'R'){
  serialEvent();
  }
  if (inputString[0] == 'R'){
  inputString = "";
  stringComplete = false;
}
  //waitForRaspberry();  // Attende handshake dal Raspberry Pi
  //while(!Serial.available())
  //waitForRaspberry();  // Attende che il Raspberry Pi sia connesso
  delay(500);
  digitalWrite(STBY, HIGH);
  s1=analogRead(A1);
  s2=analogRead(A2);
  s3=analogRead(A3);
  s4=analogRead(A6);
  s5=analogRead(A7);


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
void loop()
{
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
         Serial.println('K');
      }
      else if(inputString[0] == 'I'){
       motor1.drive(100);
       motor2.drive(-100);
       delay(3500);
       Serial.println("I&");
       inputString[0] = '\0';
      
       }
      else if(inputString[0] == 'A'){Serial.println('A');
      inputString[0] = '\0';
      }
      
      else if(inputString[0] == 'S'){
      motor1.drive(-100);
       motor2.drive(100);
       delay(2500);
       Serial.println("S&");
     inputString[0] = '\0';
      
    }
       
      else if(inputString[0] == 'D'){
       motor1.drive(100);
       motor2.drive(-100);
       delay(2500);
       Serial.println("D&");
      inputString[0] = '\0';
       }
       
      else if(inputString[0] == 'f'){informazione();}
    
    // clear the string:
    inputString = "";
    stringComplete = false;   
   }
   
  //***=============  
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

 if((s1_/2)<((ref_s1/2)-35)){a1=1; }else{a1=0;}
 if((s2_/2)<((ref_s2/2)-35)){a2=1; }else{a2=0;}
 if((s3_/2)<((ref_s3/2)-5)){a3=1; }else{a3=0;}
 if((s4_/2)<((ref_s4/2)-35)){a4=1; }else{a4=0;}
 if((s5_/2)<((ref_s5/2)-35)){a5=1; }else{a5=0;}


////Giroscopio/////



 ///comendo bianco///
while(a1==0 && a2==0 && a3==0 && a4==0 && a5==0){
  //if(last_a3==1 && last_a4==0 && last_a2==0){
   // a3=1;
   // last_a3=2;
  //}  
  motor1.drive(100);
  motor2.drive(100);
  delay(200);
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

last_a4=a4;
last_a3=a3;
last_a2=a2;

  if((millis() - millisPausaMotore) >= 100)
  {
    //informazione();
    motor1.drive(dSpeed);
    motor2.drive(sSpeed);

    millisPausaMotore = millis();
  }

  //delay(100);
  
  
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

void bianco(){
  int j=0;
 /*if(a3==1){
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
  s4=analogRead(A6);
  s5=analogRead(A7);
  s1_= ((s1 * (uint32_t)1000)/Maxs);
  s2_= ((s2 * (uint32_t)1000)/Maxs);
  s3_= ((s3 * (uint32_t)1000)/Maxs);
  s4_= ((s4 * (uint32_t)1000)/Maxs);
  s5_= ((s5 * (uint32_t)1000)/Maxs);
 if((s1_/2)<((ref_s1/2)-40)){a1=1; }else{a1=0;}
 if((s2_/2)<((ref_s2/2)-40)){a2=1; }else{a2=0;}
 if((s3_/2)<((ref_s3/2)-5)){a3=1; }else{a3=0;}
 if((s4_/2)<((ref_s4/2)-40)){a4=1; }else{a4=0;}
 if((s5_/2)<((ref_s5/2)-40)){a5=1; }else{a5=0;}

 if(a1==1 || a2==1 || a3==1 || a4==1 || a5==1){
 return;
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
 if (j==2){return;}
}
}
void waitForRaspberry() {
  Serial.println("Attesa connessione Raspberry Pi. Invia 'R' per iniziare.");
  while (true) {
    if (Serial.available() > 0) {
      char ch = Serial.read();
      if (ch == 'R') {
        Serial.println("Raspberry Pi connesso. Avvio...");
        break;
      }
    }
  }
}






//***********************-------------------------
