#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>

#define length 120 //araç uzunluğu    |bunların testlere göre 1-2cm fazla verilmesi gerekebilir
#define width 67 //araç genişliği     |

#define turning_distance 33 //dönmek için engelle arasında bırakması gereken mesafe: uzunluğun yarısı
#define motor_radius 10 //motor çubuğunun çevresi


MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;


byte commands = 0; // 00000001 = sol 00000010 = sag 00000100 = manuel 00001000 = basla 00010000 = git

const int enA = 12;
const int motleftB = 11;
const int motleftA = 10;

const int motrightA = 5;
const int motrightB = 6;

const int senA  = 4; //trigger1
const int senB = A2; //trigger2
const int senC = A3; //trigger3
const int echo1 = 7; //echo1 F
const int echo2 = 8; //echo2 R
const int echo3 = 9; //echo3 L

long sure;
double toplamyol;
uint16_t distance;
uint16_t distanceR;
uint16_t distanceL;

int speedR = 90;
int speedL = 90;
int motor_rotation = 0;
boolean turn = 0; //  0 = aracın solu kapalı
boolean could_be_the_end = 0;       

void yavasla(int, int);
void hizlan(int, int);
void motorcounter();
void turn_left();
void turn_right();

void setup() {

pinMode(A6, INPUT);
//motor pins
pinMode(motleftA, OUTPUT);
pinMode(motleftB, OUTPUT);

pinMode(motrightA, OUTPUT);
pinMode(motrightB, OUTPUT);

pinMode(enA, OUTPUT);
//motor pins

//interrupt
pinMode(2, INPUT);
attachInterrupt(digitalPinToInterrupt(2),motorcounter, FALLING);

//sensor init
pinMode(senA, OUTPUT);
pinMode(senB, OUTPUT);
pinMode(senC, OUTPUT);

//echo1 0 to trigger of sensors to assign address
digitalWrite(senC, HIGH);
 delayMicroseconds(10);
digitalWrite(senC, LOW);
digitalWrite(senB, HIGH);
 delayMicroseconds(10);
digitalWrite(senB, LOW);
digitalWrite(senA, HIGH);
 delayMicroseconds(10);
digitalWrite(senA, LOW);


//echo
  pinMode(echo1, INPUT);
  pinMode(echo2, INPUT);
  pinMode(echo3, INPUT);
  

      accelgyro.initialize();
//motor direction
  analogWrite(motleftA, 0); //low
  analogWrite(motleftB, 110); //high
  analogWrite(motrightA, 0); //low
  analogWrite(motrightB, 110); //high
//speed of motor
digitalWrite(enA, 1);
//1 sensörden mesafe alma kısmı
 sure=pulseIn(echo1,HIGH);
toplamyol=(double)sure*0.034;
distance=toplamyol/2;
//
 sure=pulseIn(echo2,HIGH);
toplamyol=(double)sure*0.034;
distanceR=toplamyol/2;

 sure=pulseIn(echo3,HIGH);
toplamyol=(double)sure*0.034;
distanceL=toplamyol/2;

if(distanceR <= turning_distance && distanceL >= turning_distance)
turn = 1;
}

void loop() {

  commands = Wire.requestFrom(8, 1);
switch (commands)
{
case 00000001:
  turn_left();
  commands = Wire.requestFrom(8, 1);
  break;
  case 00000010:
  turn_right();
  commands = Wire.requestFrom(8, 1);
  break;
  case 00000100:
  commands = Wire.requestFrom(8, 1);
  break;
  case 00010000:
  hizlan(motleftB, 90);
  hizlan(motrightB, 90);
  while (commands == 00010000)
  {commands = Wire.requestFrom(8, 1);}
  //buraya ulaşmaması lazım
  break;


default:

 sure=pulseIn(echo1,HIGH);
toplamyol=(double)sure*0.034;
distance=toplamyol/2;
//distance = SensorR.readRangeSingleMillimeters();
//Serial.print(distance);
accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

if(gz < -40){
if(speedR == 90)
yavasla(motleftB, 10);

hizlan(motrightB, 10);
}
else if(gz > 16){
if(speedL == 90)
yavasla(motrightB, 10);

hizlan(motleftB, 10);
}

while( distance <= turning_distance){ //kod burayı yakalayamadan sensör değer değiştirirse burayı if-else yaparız
if(turn){
turn_left();
while(motor_rotation * motor_radius < width); //genişliği kadar mı ilerlesin uzunlluğu kadar mı? testlere göre değiştirilmeli
 sure=pulseIn(echo2,HIGH);
toplamyol=(double)sure*0.034;
distanceR=toplamyol/2;
if(distanceR <= turning_distance)
{motor_rotation = 0;while(motor_rotation * motor_radius < width);turn_left(); turn = 0;break;}//¢  Bu break iki if içinde. iki if'den de çıkması lazım
turn_right();
while(motor_rotation * motor_radius < (length + turning_distance)); //sensör koşulu gereksiz olabilir. testlerden sonra gözden geçirilmeli
turn_right();
while(motor_rotation * motor_radius < width); //bu sefer sensörün out of range'dan çıkmasını ve normal bir değer vermesini bekliyoruz. testlere göre out of range mesafesi yerine makul bir değer girilebilir
turn_left();}


else{
turn_right();
while(motor_rotation * motor_radius < width); //genişliği kadar mı ilerlesin uzunlluğu kadar mı? testlere göre değiştirilmeli
 sure=pulseIn(echo3,HIGH);
toplamyol=(double)sure*0.034;
distanceL=toplamyol/2;
if(distanceL <= turning_distance)
{motor_rotation = 0;while(motor_rotation * motor_radius < width);turn_right(); turn = 1; break;}//¢  Bu break iki if içinde. iki if'den de çıkması lazım
turn_left();
while(motor_rotation * motor_radius < length + turning_distance); //sensör koşulu gereksiz olabilir. testlerden sonra gözden geçirilmeli
turn_left();
while(motor_rotation * motor_radius < width); //bu sefer sensörün out of range'dan çıkmasını ve normal bir değer vermesini bekliyoruz. testlere göre out of range mesafesi yerine makul bir değer girilebilir
turn_right();
}
break;
}
}
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
yavasla(motrightB, 0);
hizlan(motrightA, 90);
/*int rotation_path = 2 * PI * (length / 2) / 4 ; // çevre /4 çünkü 90 derece dönecek = çevrenin dörtte biri kadar.
int current_path_taken = motor_radius * motor_rotation;
while (current_path_taken < rotation_path);*/
while(count < 5){delay(1000);count++;}
yavasla(motrightA, 0);
hizlan(motrightB, 90);
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