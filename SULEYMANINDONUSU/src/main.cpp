#include <Arduino.h>
#include <VL53L0X.h>


#define length 120 //araç uzunluğu    |bunların testlere göre 1-2cm fazla verilmesi gerekebilir
#define width 50 //araç genişliği     |

#define turning_distance 33 //dönmek için engelle arasında bırakması gereken mesafe: genişliğin yarısı
#define motor_radius 36 //motor çubuğunun çevresi

int motor_rotation = 0;
int speed = 255;

int speedR = 110;
int speedL = 110;

const int enA = 12; // µ
const int motleftA = 11; // µ
const int motleftB = 10; //µ

const int motrightA = 5;
const int motrightB = 6;
void turn_left(); //µ
void turn_right(); //µ
void yavasla(int, int);
void hizlan(int, int);
void setup()
{
pinMode(motleftA, OUTPUT);
pinMode(motleftB, OUTPUT);

pinMode(motrightA, OUTPUT); //µ
pinMode(motrightB, OUTPUT); //µ
pinMode(enA, OUTPUT);

//interrupt

  analogWrite(motleftA, 0); //low
  analogWrite(motleftB, 210); //high
  analogWrite(motrightA, 0); //low
  analogWrite(motrightB, 240); //high

  digitalWrite(enA, 1);
}

void loop()
{
  delay(5000);
  turn_right();
}

void motorcounter(){
motor_rotation++;

}

void turn_left(){
 int count = 0;
 motor_rotation = 0;
yavasla(motleftB, 0);
hizlan(motleftA, 90);
/*int rotation_path = 2 * PI * (length / 2) / 4 ; // çevre /4 çünkü 90 derece dönecek = çevrenin dörtte biri kadar.
int current_path_taken = motor_radius * motor_rotation;
while (current_path_taken < rotation_path);*/
while(count < 5){delay(1000);count++;}
yavasla(motleftA, 0);
hizlan(motleftB, 90);
motor_rotation = 0; }


void turn_right(){
 int count = 0;
 motor_rotation = 0;
yavasla(motrightB, 50);
/*int rotation_path = 2 * PI * (length / 2) / 4 ; // çevre /4 çünkü 90 derece dönecek = çevrenin dörtte biri kadar.
int current_path_taken = motor_radius * motor_rotation;
while (current_path_taken < rotation_path);*/
while(count < 5){delay(1000);count++;}
hizlan(motrightB, 240);
motor_rotation = 0;  
}

void yavasla(int yon, int miktar){
  int speed = 90;
  if(yon < 9)
  speed = speedR;
  else
  speed = speedL;
while(speed > miktar){speed--; analogWrite(yon, speed); delay(2);}
  if(yon < 9)
  speedR = speed;
  else
  speedL = speed;
}
void hizlan(int yon, int miktar){
    int speed = 90;
  if(yon < 9)
  speed = speedR;
  else
  speed = speedL;
while(speed < miktar){speed++; analogWrite(yon, speed); delay(2);}
  if(yon < 9)
  speedR = speed;
  else
  speedL = speed;
}
