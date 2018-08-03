/*
This program used for controlling servos of Unicorn3.0 and later versions(radial 5-arm maze)

created in the summer of 2016
by Zou Shimin & Zhou Xuehan

modified 09 Jan 2017: added State6 as a marker to control rotator(if digitalRead(33)==LOW, reset state6)
modified 7th Nov, 2017: open angle was changed from 90 (or 0) to 85 (or 5) to protect servo from over-rotating

*/
#include <Servo.h>
Servo DOOR1;
Servo DOOR2;
Servo DOOR3;
Servo DOOR4;
Servo DOOR5;
Servo DOOR6;
Servo ROTATOR;
  int State1=0;
  int State2=0;
  int State3=0;
  int State4=0;
  int State5=0;
  int State6=0;
  int mPosition;
  int currentPosition;

void setup() {
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(23,INPUT);
  pinMode(25,INPUT);
  pinMode(27,INPUT);
  pinMode(29,INPUT);
  pinMode(31,INPUT);
  pinMode(33,INPUT);
  
  DOOR1.attach(2);
  DOOR2.attach(3); 
  DOOR3.attach(4); 
  DOOR4.attach(5); 
  DOOR5.attach(6); 
  DOOR6.attach(7); 
  ROTATOR.attach(8);
 }

void loop() {
  if(State1==0 && digitalRead(23)== HIGH){
  DOOR1.write(5);
  State1=1;
  }
  else if(State2==0 && digitalRead(25)== HIGH){
  DOOR2.write(5);
  State2=1;
  }
  else if(State3==0 && digitalRead(27)== HIGH){
  DOOR3.write(5);
  State3=1;
  }
  else if(State4==0 && digitalRead(29)== HIGH){
  DOOR4.write(5);
  State4=1;
  }
  else if(State5==0 && digitalRead(31)== HIGH){
  DOOR5.write(0);DOOR6.write(90);delay(200);
  State5=1;
  }

  else if(State1==1 && digitalRead(23)== LOW){
  DOOR1.write(90);
  State1=0;
  }                                                                                                       
  else if(State2==1 && digitalRead(25)== LOW){
  DOOR2.write(90);
  State2=0;
  }
  else if(State3==1 && digitalRead(27)==LOW){
  DOOR3.write(90);
  State3=0;
  }
  else if(State4==1 && digitalRead(29)== LOW){
  DOOR4.write(90);
  State4=0;
  }
  else if(State5==1 && digitalRead(31)== LOW){
  DOOR6.write(0);
  DOOR5.write(90);delay(200);
  State5=0;
  }
   else if(State6==1 && digitalRead(33)== LOW){
    State6=0;
  }
  else if(State6==0&&digitalRead(33)==HIGH){
    mPosition = (rand()%4+3)*30;
    if(currentPosition != mPosition){
          ROTATOR.write(mPosition);
          currentPosition = mPosition;
          delay(500);
          State6=1;
      }
    else; 
    }
 
}
  
 


