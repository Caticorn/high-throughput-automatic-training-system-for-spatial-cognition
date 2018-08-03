/*
This work only for Spatial in Unicorn3.0 and Unicorn4.0 (5-arm maze)
*(1)Delayed non-match to sample
*(2)Forced alternation
*(3)Delayed alternation
*ONLY TT or YY trials in each training day
*created back in the mists of time (one summer, 2016)
 by Zhou Xuehan & Zou Shimin
 based on Lai Zhenqi's pseudo code and Zhang Xiaoxing's example for olfactory WM task
*modified 26th Apr. 2018
 by Zou Shimin
*/

#include <SoftwareSerial.h>
#include "unicorn.h"
#include "Arduino.h"
#include "TimerOne.h"
#include "trainingArray.h"

void setup() {
  pinMode(13, OUTPUT); //LED
  pinMode(22, OUTPUT); // mosfet arm1,water pump
  pinMode(23, OUTPUT); // engine arm1,door
  pinMode(24, OUTPUT); // mosfet arm2
  pinMode(25, OUTPUT); // engine arm2
  pinMode(26, OUTPUT); // mosfet arm3
  pinMode(27, OUTPUT); // engine arm3
  pinMode(28, OUTPUT); // mosfet arm4
  pinMode(29, OUTPUT); // engine arm4
  pinMode(30, OUTPUT); // mosfet arm home
  pinMode(31, OUTPUT); // engine arm home
  pinMode(32, OUTPUT); // 
  pinMode(33, OUTPUT); // engine arm home,mirror door
  pinMode(34, OUTPUT); //  
  pinMode(35, OUTPUT); // engine rotatory plate
  pinMode(36, OUTPUT); //laser BNC
  pinMode(37, OUTPUT); 
  pinMode(38, OUTPUT); 
  pinMode(39, OUTPUT);
  pinMode(40, OUTPUT);//LED1 
  pinMode(41, OUTPUT);//LED2
  pinMode(42, OUTPUT);//LED3
  pinMode(43, OUTPUT);
  pinMode(44, OUTPUT);
  pinMode(45, OUTPUT);
  pinMode(46, OUTPUT); 
  pinMode(47, OUTPUT); 
  pinMode(48, OUTPUT);
  pinMode(49, OUTPUT);
  pinMode(50, OUTPUT);
  pinMode(51, OUTPUT); //LASER
  pinMode(52, OUTPUT); 
  pinMode(53, OUTPUT); 
  
  digitalWrite(52,LOW);
  digitalWrite(53,LOW);
  digitalWrite(51,LOW);

  Serial.begin(9600);
  byte init[]={0,1,2,3};
  Serial.write(init,4);
  
}

void loop() {
   int n = getFuncNumber(4);
  randomSeed(analogRead(0));
  switch(n/100){
    case 14:// test
    unicornFunction(n);
    break;
    case 15:// training
    unicornFunction(n);
    break;
   
  }

}

