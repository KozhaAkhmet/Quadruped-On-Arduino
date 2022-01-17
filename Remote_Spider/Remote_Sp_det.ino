#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>
#include <string.h>

#define SERVO_FREQ 60

RF24 radio(9, 10);
const byte address[6] = "10101";
int number=0;

double claw = 85, connecter = 55, initial = 33, last_posx = 60, last_posy = 60, last_posz = 60;
struct Data_Package {
  byte j1PotX;
  byte j1PotY;
  byte j2PotX;
  byte j2PotY;
  byte Arrows;
  byte Circle;
  byte Square;
  byte Triangle;
  byte Ex;
};
Data_Package data;
Adafruit_PWMServoDriver Servo = Adafruit_PWMServoDriver();
void setup() {
  Servo.begin();
  Servo.setPWMFreq(SERVO_FREQ);
  yield();
  
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
  Default();
  
}
void loop() {
  double ServoDeg, a;
  
  char cons = Serial.read();
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));
    Serial.println(data.j1PotX);
    Serial.println(data.j2PotY);
    if(data.Square==1){
      int j1x=map(data.j1PotX,0,255,40,-40),j1y=map(data.j1PotY,0,255,-40,40);
      BodyMove(j1x,j1y,60);
   }
 


    if (cons == '+') {
      number=number + 5;
      Servo.setPWM(8, 0, number);
      Serial.println(number);
    } else if (cons == '-') {
      number=number -5;
      Servo.setPWM(8, 0, number);
      Serial.println(number);
    } else if (cons == '0') {
      Default();
    } else if (cons == '1' || cons == 't') {
      Twerk();
    } else if (cons == '2') {
      Dance();
    } else if (cons == '3') {
      Walk();
    } else if (cons == '4') {
    }
  }
 }
void Default() {
  Pos1(40, 40, 60);
  Pos2(40, 40, 60);
  Pos3(40, 40, 60);
  Pos4(40, 40, 60);
}
void Twerk() {
  for (int i = 0 ; i <= 41 ; i++) {
    Pos1(0, 60, 60);
    Pos2(60, 0, 60);
    Pos3(0, 60, 60);
    Pos4(60, 0, 60);
    delay(200);
    Pos3(0, 60, 40);
    Pos4(60, 0, 40);
    delay(200);
  }
}
void Dance() {
  for (int i = 60 ; i > 20 ; i -= 3 ) {
    Pos1(60, 60, i);
    Pos2(60, 60, i);
    Pos3(60, 60, i);
    Pos4(60, 60, i);
    delay(20);
  }
  for (int i = 20 ; i <= 60 ; i += 3 ) {
    Pos1(60, 60, i);
    Pos2(60, 60, i);
    Pos3(60, 60, i);
    Pos4(60, 60, i);
    delay(20);
  }
}
void Walk() {
  int t = 400;
  BodyMove(0, -10, 60);
  delay(t);
  Pos4(70, 20, 40);
  delay(t);
  Pos4(70, 20, 60);
  delay(t);
  Pos2(70, 120, 0);
  delay(t);
  Pos2(70, 120, 60);
  delay(t);
  Pos1(70, 20, 60);
  Pos2(70, 70, 60);
  Pos3(70, 120, 60);
  Pos4(70, 70, 60);
  delay(t);
  Pos3(70, 20, 40);
  delay(t);
  Pos3(70, 20, 60);
  delay(t);
  Pos1(70, 120, 0);
  delay(t);
  Pos1(70, 120, 60);
  delay(t);
  Pos1(70, 70, 60);
  Pos2(70, 20, 60);
  Pos3(70, 70, 60);
  Pos4(70, 20, 60);
}
void Walktest(int xchange[4], int ychange[4]) {
  int i, t = 600, lastposx[4] = {70, 70, 70, 70}, lastposy[4] = {70, 70, 70, 70};
  Pos4(70, (lastposy[3] - ychange[3]), 40);
  delay(t);
  Pos4(70, lastposy[3] = (lastposy[3] - ychange[3]), 60);
  delay(t);
  Pos2(70, (lastposy[1] + ychange[1]), 0);
  delay(t);
  Pos2(70, lastposy[1] = (lastposy[1] + ychange[1]), 60);
  delay(t);
  Pos1(70, lastposy[0] = (lastposy[0] - ychange[0]), 60);
  Pos2(70, lastposy[1] = (lastposy[1] - ychange[1]), 60);
  Pos3(70, lastposy[2] = (lastposy[2] + ychange[2]), 60);
  Pos4(70, lastposy[3] = (lastposy[3] + ychange[3]), 60);
  delay(t);
  Pos3(70, lastposy[2] - 2 * ychange[2], 40);
  delay(t);
  Pos3(70, lastposy[2] = (lastposy[2] - 2 * ychange[2]), 60);
  delay(t);
  Pos1(70, lastposy[0] + 2 * ychange[0], 0);
  delay(t);
  Pos1(70, lastposy[0] = (lastposy[0] + 2 * ychange[0]), 60);
  delay(t);
  Pos1(70, lastposy[0] - ychange[0], 60);
  Pos2(70, lastposy[1] - ychange[1], 60);
  Pos3(70, lastposy[2] + ychange[2], 60);
  Pos4(70, lastposy[3] - ychange[3], 60);
  delay(t);
}
void Pos1(float posx, float posy, float posz) {
  double al, al1, al2, bet, gam, L, L1 = sqrt(posx * posx + posy * posy), VServo3, VServo2, VServo1;
  L = sqrt(posz * posz + (L1 - initial) * (L1 - initial));
  al1 = (180 * (acos(posz / L)) / PI);
  al2 = (180 * (acos((claw * claw - connecter * connecter - L * L) / (-2 * connecter * L)))) / PI;
  gam = ((180 * (atan(posx / posy))) / PI + 45);
  bet = (180 * acos((L * L - claw * claw - connecter * connecter) / (-2 * claw * connecter))) / PI;
  al = (al1 + al2);
  VServo1 = map(gam, 0, 180, 130, 650);
  VServo2 = map(al, 0, 180, 95, 620);
  VServo3 = map(bet, 0, 180, 95, 630);
  //Serial.println("=====================");
  //Serial.print(gam);
  //Serial.print("\t");
  //Serial.print(al);
  //Serial.print("\t");
  //Serial.print(bet);
  //Serial.print("\n");
  Servo.setPWM(0, 0, VServo1);
  Servo.setPWM(1, 0, VServo2);
  Servo.setPWM(2, 0, VServo3);
}
void Pos2(float posx, float posy, float posz) {
  double al, al1, al2, bet, gam, L, L1 = sqrt(posx * posx + posy * posy), VServo3, VServo2, VServo1;
  L = sqrt(posz * posz + (L1 - initial) * (L1 - initial));
  al1 = (180 * (acos(posz / L)) / PI);
  al2 = (180 * (acos((claw * claw - connecter * connecter - L * L) / (-2 * connecter * L)))) / PI;
  gam = ((180 * (atan(posy / posx))) / PI + 45);
  bet = (180 * acos((L * L - claw * claw - connecter * connecter) / (-2 * claw * connecter))) / PI;
  al = 180 - (al1 + al2);
  VServo1 = map(gam, 0, 180, 95, 650);
  VServo2 = map(al, 0, 180, 95, 630);
  VServo3 = map(bet, 0, 180, 620, 95);
  //Serial.print(gam);
  //Serial.print("\t");
  //Serial.print(al);
  //Serial.print("\t");
  //Serial.print(bet);
  //Serial.print("\n");
  Servo.setPWM(3, 0, VServo1);
  Servo.setPWM(4, 0, VServo2);
  Servo.setPWM(5, 0, VServo3);
}
void Pos3(float posx, float posy, float posz) {
  double al, al1, al2, bet, gam, L, L1 = sqrt(posx * posx + posy * posy), VServo3, VServo2, VServo1;
  L = sqrt(posz * posz + (L1 - initial) * (L1 - initial));
  al1 = (180 * (acos(posz / L)) / PI);
  al2 = (180 * (acos((claw * claw - connecter * connecter - L * L) / (-2 * connecter * L)))) / PI;
  gam = ((180 * (atan(posy / posx))) / PI + 45);
  bet = (180 * acos((L * L - claw * claw - connecter * connecter) / (-2 * claw * connecter))) / PI;
  al = 180 - (al1 + al2);
  VServo1 = map(gam, 0, 180, 130, 650);         //Temmp changed from 110 650
  VServo2 = map(al, 0, 180, 95, 630); //2000
  VServo3 = map(bet, 0, 180, 630, 95);
  //Serial.print(gam);
  //Serial.print("\t");
  //Serial.print(al);
  //Serial.print("\t");
  //Serial.print(bet);
  //Serial.print("\n");
  Servo.setPWM(6, 0, VServo1);
  Servo.setPWM(7, 0, VServo2);
  Servo.setPWM(8, 0, VServo3);
}
void Pos4(float posx, float posy, float posz) {
  double al, al1, al2, bet, gam, L, L1 = sqrt(posx * posx + posy * posy), VServo3, VServo2, VServo1;
  L = sqrt(posz * posz + (L1 - initial) * (L1 - initial));
  al1 = (180 * (acos(posz / L)) / PI);
  al2 = (180 * (acos((claw * claw - connecter * connecter - L * L) / (-2 * connecter * L)))) / PI;
  gam = ((180 * (atan(posx / posy))) / PI + 45);
  bet = (180 * acos((L * L - claw * claw - connecter * connecter) / (-2 * claw * connecter))) / PI;
  al = (al1 + al2);
  VServo1 = map(gam, 0, 180, 95, 620);
  VServo2 = map(al, 0, 180, 95, 630); //2000
  VServo3 = map(bet, 0, 180, 95, 630);
  //Serial.print(gam);
  //Serial.print("\t");
  //Serial.print(al);
  //Serial.print("\t");
  //Serial.print(bet);
  //Serial.print("\n");
  Servo.setPWM(9, 0, VServo1);
  Servo.setPWM(10, 0, VServo2);
  Servo.setPWM(11, 0, VServo3);
}
void BodyMove(int posx, int posy, int posz) {
  Pos1(60 + posx, 60 - posy, posz);
  Pos2(60 - posx, 60 - posy, posz);
  Pos3(60 + posx, 60 + posy, posz);
  Pos4(60 - posx, 60 + posy, posz);
}
void BodyMovetest() {
    int Speed=100;
    double posz = sin(millis()/Speed);
    double posy = cos(millis()/Speed);
    double posx = sin(millis()/Speed);

    if(posz<=60)posz=60;
    
    //BodyMove(posx*10,posy*10,60);
    if( posx<0 && posy<0){
      Pos2( 70, sin(millis()/Speed)*30+70, cos(millis()/Speed)*20+60);
    }else if( posx<0 && posy>0){
      //Pos3( 70, sin(millis()/Speed+PI/2)*30+70 , cos(millis()/Speed+PI/2)*20+60);
    }else if( posx>0 && posy>0){
      //Pos4( 70, sin(millis()/Speed+PI)*30+70  , cos(millis()/Speed + PI)*20+60);
    }else if( posx>0 && posy<0){
      //Pos1( 70, sin(millis()/Speed+3*PI/2)*30+70,cos(millis()/Speed + 3*PI/2)*20+60);
    }
    //Pos1(posx,posy,posz);
    
    delay(100);
}
