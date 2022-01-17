/*
 * Coded by Kozha Akhmet Abdramanov
 * Quadruped V1 code
*/

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>
#include <string.h>
//#include "Quadruped.h"

#define PWM_FREQ 60                            //PWM Frequency

#define claw      83                           //Defining Leg`s length in mm
#define connecter 55
#define initial   33               
                             
RF24 radio(9, 10);                             //Defining NRF24L01 pins

const byte address[6] = "10101";               //Address of rc communication

struct Data_Package {                          //Recieved package
  byte j1PotX;
  byte j1PotY;
  byte j2PotX;
  byte j2PotY;
  byte Arrows;
  byte Circle;
  byte Square;
  byte Triangle;
  byte Ex;
};Data_Package data;

Adafruit_PWMServoDriver PWM = Adafruit_PWMServoDriver();           //Defining PWMs

struct Ang {
    double al;
    double bet;
    double gam;
};
struct Pos {
    double x;
    double y;
    double z;
};
struct Servo {
    int id;
    int range[2];
};
class Leg {                                                        //Making Leg claas and its ingredients
    public:
        Ang ang;
        Pos pos;
        Servo servo[3];
        void toPos(double posX, double posY, double posZ);
        void toAng(double al, double bet , double gam );
        void Step (double posX, double posY, double posZ);
};Leg leg[4];

void Leg::toPos(double posX, double posY, double posZ) {           //Inverse kinematic
    double al, bet, gam, L, L1 = sqrt(posX * posX + posY * posY);
    L = sqrt(posZ * posZ + (L1 - initial) * (L1 - initial));
    al = (180 * (acos((claw * claw - connecter * connecter - L * L) / (-2 * connecter * L)))) / PI +
         (180 * (acos(posZ / L)) / PI);
    gam = (((180 * (atan(posX / posY))) / PI + 45));
    bet = (180 * acos((L * L - claw * claw - connecter * connecter) / (-2 * claw * connecter))) / PI;
    ang.al = al;
    ang.gam = gam;
    ang.bet = bet;
    pos.x = posX;
    pos.y = posY;
    pos.z = posZ;
    PWM.setPWM(this->servo[0].id  ,0,  map(gam ,0,180,   this->servo[0].range[0],  this->servo[0].range[1]));
    PWM.setPWM(this->servo[1].id  ,0,  map(al  ,0,180,   this->servo[1].range[0],  this->servo[1].range[1]));
    PWM.setPWM(this->servo[2].id  ,0,  map(bet ,0,180,   this->servo[2].range[0],  this->servo[2].range[1]));
}

void Leg::toAng(double al, double bet , double gam ){             
    PWM.setPWM(this->servo[0].id  ,0,  gam);
    PWM.setPWM(this->servo[1].id  ,0,  al );
    PWM.setPWM(this->servo[2].id  ,0,  bet);
}

void Leg::Step(double posX, double posY, double posZ){
  double R=sqrt((posX-pos.x)*(posX-pos.x) + (posY-pos.y)*(posY-pos.y) + (posZ-pos.z)*(posZ-pos.z))/2;
  double tmpx=pos.x,tmpy=pos.y,tmpz=pos.z, sinus;
  while(!(pos.x + R*cos(millis()/500) == posX)){
  sinus= sin(millis()/500) > 0 ? sin(millis()/500) : 0 ;
  toPos(tmpx + R - R*cos(millis()/500), tmpy, tmpz + R*sinus);
  }
}

void setup() {
  PWM.begin();
  PWM.setPWMFreq(PWM_FREQ);
  int i,j;                                                 
  for ( i = 0 ; i < 4; i++) {                                      //Setups for servos
        for( j = 0 ; j < 3; j++){
            leg[i].servo[j].id=3*i+j;
        }
    }
    leg[0].servo[0].range[0]= 130;  leg[0].servo[0].range[1]= 650; //Defining servo`s ranges
    leg[0].servo[1].range[0]= 95 ;  leg[0].servo[1].range[1]= 620;
    leg[0].servo[2].range[0]= 95 ;  leg[0].servo[2].range[1]= 630;
    
    leg[1].servo[0].range[0]= 650;  leg[1].servo[0].range[1]= 95 ;
    leg[1].servo[1].range[0]= 630;  leg[1].servo[1].range[1]= 95 ;
    leg[1].servo[2].range[0]= 620;  leg[1].servo[2].range[1]= 95 ;

    leg[2].servo[0].range[0]= 650;  leg[2].servo[0].range[1]= 130;
    leg[2].servo[1].range[0]= 630;  leg[2].servo[1].range[1]= 95 ;
    leg[2].servo[2].range[0]= 630;  leg[2].servo[2].range[1]= 95 ;

    leg[3].servo[0].range[0]= 95 ;  leg[3].servo[0].range[1]= 620;
    leg[3].servo[1].range[0]= 95 ;  leg[3].servo[1].range[1]= 630;
    leg[3].servo[2].range[0]= 95 ;  leg[3].servo[2].range[1]= 630;
    
  yield();

  Serial.begin(9600);                                               //Serial port communication
  
  radio.begin();                                                    //RC setup
  radio.openReadingPipe(0, address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
                                                   
  defaultPos();
}
void loop() {
  
  radioRead();
}
void serialLog(){                                                   //Serial communication
  char serialInput = Serial.read();
  
  Serial.println(data.j1PotX);
  Serial.println(data.j2PotY);
  
    if (serialInput == '+') {
           
    } else if (serialInput == '0') {
      defaultPos();
    } else if (serialInput == '1') {
      
    } else if (serialInput == '2') {
      
    } else if (serialInput == '3') {
      
    } else if (serialInput == '4') {
      
    }
}
void radioRead(){                                                   //Begining to read radio waves
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));
      
    }     if(data.Triangle==1){
      walk();
      
    }else if(data.Square==1  ){
      int j1x=map(data.j1PotX,0,255,40,-40),j1y=map(data.j1PotY,0,255,-40,40);
      bodyMove(j1x,j1y,60);
      
    }else if(data.Circle==1  ){
      
    }else if(data.Ex==1      ){
      testt();
    }else if(data.Arrows==1  ){
      
    }
}
void defaultPos() {                                                 //Default leg positions
  for(int i=0; i<4 ; i++){
    leg[i].toPos(40,40,60);
  }
}
void walk() {                                                       //Manuel Walking gait
  int vel =200;
  leg[3].toPos(40, 20, 40);
  delay(vel);
  leg[3].toPos(40, 20, 60);
  delay(vel);
  leg[1].toPos(40, 120, 0);
  delay(vel);
  leg[1].toPos(40, 120, 60);
  delay(vel);
  leg[0].toPos(40, 20, 60);
  leg[1].toPos(40, 70, 60);
  leg[2].toPos(40, 120, 60);
  leg[3].toPos(40, 70, 60);
  delay(vel);
  leg[2].toPos(40, 20, 40);
  delay(vel);
  leg[2].toPos(40, 20, 60);
  delay(vel);
  leg[0].toPos(40, 120, 0);
  delay(vel);
  leg[0].toPos(40, 120, 60);
  delay(vel);
  leg[0].toPos(40, 70, 60);
  leg[1].toPos(40, 20, 60);
  leg[2].toPos(40, 70, 60);
  leg[3].toPos(40, 20, 60);
  delay(vel);
}
void bodyMove(int posx, int posy, int posz) {                        //Body movement 
  leg[0].toPos(40 + posx, 40 - posy, posz);
  leg[1].toPos(40 - posx, 40 - posy, posz);
  leg[2].toPos(40 + posx, 40 + posy, posz);
  leg[3].toPos(40 - posx, 40 + posy, posz);
}
void testt(){
  leg[0].Step(130,40,60);
}
