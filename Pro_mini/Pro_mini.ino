#include <SPI.h>         
#include "nRF24L01.h"     
#include "RF24.h"         
#include <stdio.h>
RF24 radio(9, 10); 

const byte address[6] = "10101";
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
void setup() {
  Serial.begin(9600); 
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(4,INPUT);
  pinMode(5,INPUT);
  pinMode(6,INPUT);
  pinMode(7,INPUT);
  pinMode(8,INPUT);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);
  
  radio.begin();
  radio.openWritingPipe(address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);

}

void loop() {
  Variables();
  data.j1PotX = map(analogRead(A5), 0, 1023, 0,255 ); // Convert the analog read value from 0 to 1023 into a BYTE value from 0 to 255
  data.j1PotY = map(analogRead(A3), 0, 1023, 0, 255);
  data.j2PotX = map(analogRead(A4), 0, 1023, 0, 255);
  data.j2PotY = map(analogRead(A1), 0, 1023, 0, 255);
  data.Arrows = analogRead(A0);
  data.Circle = digitalRead(8);
  data.Square = digitalRead(5);
  data.Triangle = digitalRead(6);
  data.Ex = digitalRead(7);
  radio.write(&data, sizeof(Data_Package));
}
/*int Arrows(){
  int arr=analogRead(A0);
  const char UP[]="A_U",RIGHT[]="A_R",LEFT[]="A_L",DOWN[]="A_D";
  if((40<=arr && arr<=80))
    return radio.write(&UP,sizeof(UP));
  else if((700<=arr && arr<=850))
    return radio.write(&RIGHT,sizeof(RIGHT));
  else if((120<=arr && arr<=230))
    return radio.write(&LEFT,sizeof(LEFT));
  else if((450<=arr && arr<=570))
    return radio.write(&DOWN,sizeof(DOWN));
  delay(10);
}
int Shape(){
   const char Square[10]="Square",Triangle[10]="Triangle",Circle[10]="Circle",Ex[10]="Ex";
  if(digitalRead(5)){
    return radio.write(&Square,sizeof(Square));
  }else if(digitalRead(6)){
    return radio.write(&Triangle,sizeof(Triangle));
  }else if(digitalRead(8)){
    return radio.write(&Circle,sizeof(Circle));
  }else if(digitalRead(7)){
    return radio.write(&Ex,sizeof(Ex));
  }
  delay(10);
}
void LeftJoystick(){
  long int X[10]={map(analogRead(A5),0,1023,100,-100)};
  radio.write(&X,sizeof(X)); //X
  //map(analogRead(A3),0,1023,-100,100); //Y
}
void RightJoystick(){
  
}*/
/*void dump_radio_status_to_serialport(uint8_t status)
{
  Serial.print("Enrf24 radio transceiver status: ");
  switch (status) {
    case ENRF24_STATE_NOTPRESENT:
      Serial.println("NO TRANSCEIVER PRESENT");
      break;

    case ENRF24_STATE_DEEPSLEEP:
      Serial.println("DEEP SLEEP <1uA power consumption");
      break;

    case ENRF24_STATE_IDLE:
      Serial.println("IDLE module powered up w/ oscillators running");
      break;

    case ENRF24_STATE_PTX:
      Serial.println("Actively Transmitting");
      break;

    case ENRF24_STATE_PRX:
      Serial.println("Receive Mode");
      break;

    default:
      Serial.println("UNKNOWN STATUS CODE");
  }
}*/
void Variables(){
  Serial.print("\n X:");
  Serial.print(map(analogRead(A5),0,1023,100,-100));
  Serial.print("\t Y:");
  Serial.print(map(analogRead(A3),0,1023,-100,100));
  Serial.print("\t X:");
  Serial.print(map(analogRead(A4),0,1023,100,-100));
  Serial.print("\t Y:");
  Serial.print(map(analogRead(A1),0,1023,-100,100));
  Serial.print("\t RB:");
  Serial.print(digitalRead(3));
  Serial.print(" LB:");
  Serial.print(digitalRead(2));
  Serial.print("\t Arrows:");
  Serial.print(analogRead(A0));
  Serial.print("\t Square:");
  Serial.print(digitalRead(5));
  Serial.print("\t Triangle:");
  Serial.print(digitalRead(6));
  Serial.print("\t Circle:");
  Serial.print(digitalRead(8));
  Serial.print("\t Ex:");
  Serial.print(digitalRead(7));
}
