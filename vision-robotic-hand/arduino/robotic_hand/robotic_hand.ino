#include <Servo.h>

Servo servo0;
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
int servo_angle=0;
void setup() {
  Serial.begin(9600);
  
  servo0.attach(3);  // Attacca il servo0 al pin 3
  servo1.attach(5);  // Attacca il servo1 al pin 5
  servo2.attach(6);  // Attacca il servo2 al pin 6
  servo3.attach(9);  // Attacca il servo3 al pin 9
  servo4.attach(10); // Attacca il servo4 al pin 10
  
  servo0.write(100);
  servo1.write(servo_angle);
  servo2.write(servo_angle);
  servo3.write(servo_angle);
  servo4.write(servo_angle);
  
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    if (data.length() == 5) {
      
      for (int i = 0; i < 5; i++) {
        char state = data.charAt(i);
        if (state == '1') {
          servo_angle = 100; // Dito piegato
        } else {
          servo_angle = 0;   // Dito aperto
        }
        switch (i) {
          case 0:
            if(servo_angle==0){servo0.write(100);}     
          if(servo_angle==100){servo0.write(2);}
            break;
          case 1:
          if(servo_angle==0){servo1.write(servo_angle);}     
          if(servo_angle==100){servo1.write(servo_angle+20);}       
            break;
          case 2:
            if(servo_angle==0){servo2.write(servo_angle);}     
          if(servo_angle==100){servo2.write(servo_angle+150);}
            break;
          case 3:
           if(servo_angle==0){servo3.write(servo_angle);}     
          if(servo_angle==100){servo3.write(servo_angle+40);}
            break;
          case 4:
            servo4.write(servo_angle);
            break;
        }
      }
    }
  }
  //delay(100); // Aggiungi un piccolo ritardo per dare tempo alla seriale di leggere
}
