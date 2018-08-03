#include <SoftwareSerial.h>
#include "unicorn.h"
#include "Arduino.h"
#include "trainingArray.h"

int mState,mPhase;
int mSample,mChoice,mSample2, mChoice2,mLaser,mCatch;
int mDelayDuration = 1;
int mDelayDuration2 = 1;
int mDelayDuration3 = 1;
int mPairedArm,mNonPairedArm,mDistractor;
int choiceOne,choiceTwo,choiceThree;
int taskType;
int result = 1 ;//used in AFTER_CHOICE phase: 1 for hit (water supply in home arm) and 0 for false
unsigned long targetTime;  
boolean laserStatus=false;

// water volume
const int luredWater = 60; //3.5uL//sample arm & home arm//duration: 60ms
const int hitWater = 100; //5.5uL//choice arm//duration: 100ms
const int homeWater = 60; 
const int sampleWater = 60; 
const int distractorWater = 60; 
const int choiceWater = 100;
// threshold of infrared
unsigned int passBound[]={0,200,  200,  200,  200,  150};//test case: 1411
// in catch trial: no distractor, only delay



void callReset(){
  Timer1.detachInterrupt();
  Serial.read();
  delay(100);
  pinMode(52,OUTPUT);
  pinMode(53,OUTPUT);
  digitalWrite(52,LOW);
  digitalWrite(53,HIGH);
}
int getFuncNumber(int targetDigits){
    int bitSet[targetDigits];
    int bitValue[targetDigits];
    unsigned int n;
    int iter;
    int iter1;

    for (iter = 0; iter < targetDigits; iter++) {
        bitSet[iter] = 0;
        bitValue[iter] = -6;
    }

    for (iter = 0; iter < targetDigits; iter++) {
        while (!bitSet[iter]) {
            if (Serial.peek() > 0) {

        if(Serial.peek()==0x2a){
          callReset();
        }
                bitValue[iter] = Serial.read() - 0x30;
                bitSet[iter] = 1;
            }
        }
        serialSend(SpCome, bitValue[iter]);
    }
    n = 0;
    for (iter1 = 0; iter1 < targetDigits; iter1++) {
        n = n * 10 + bitValue[iter1];
    }
    return n;
}
void serialSend(int type, int value){
  byte toSend[]= {0x55, (byte) type, (byte) value, 0xAA};
  Serial.write(toSend,4);
}

// laser control
void laserOn(){ 
  if (laserStatus==false) {
    digitalWrite(36, HIGH);
    serialSend(SpLaser, 1);
    laserStatus=true;
  }
}
void laserOff(){ 
  if (laserStatus==true) {
    digitalWrite(36, LOW);
    serialSend(SpLaser, 0);
    laserStatus=false;
  }
}
void laserInTrialPhase(int laserOnOrOff){//laserOnOrOff: 1 for On, 0 for Off
  switch(laserOnOrOff){
    case 0:
    if (laserStatus==true) {
      digitalWrite(36, LOW);
      digitalWrite(13, LOW);// LED indicator, only for testing // delete this line in formal version
      serialSend(SpLaser, 0);
      laserStatus=false;
    }
    break;
    case 1:
    if (laserStatus==false) {
      digitalWrite(36, HIGH);
      digitalWrite(13, HIGH);// LED indicator, only for testing // delete this line in formal version
      serialSend(SpLaser, 1);
      laserStatus=true;
    }
    break; 
  }
}

// LED signal for indicating trial-phase (video)
void phaseState(int aimToArm, int firstHalf){
  digitalWrite(40,aimToArm == 1?HIGH:LOW); //LED1 on:aim to arm (sample phase & choice phase); off: aim to home (delay & ITI)
  digitalWrite(41,firstHalf == 1?HIGH:LOW); //LED2 on: sample phase & delay; off: choice phase & ITI;
}

// pump control
void valveOn(int valve){
  digitalWrite(valve+valve+20, HIGH);
}
void valveOff(int valve){
  digitalWrite(valve+valve+20, LOW);
}
void rewardWater(int valve, int waterTime){
    digitalWrite(valve+valve+20, HIGH);
    delay(waterTime);
    digitalWrite(valve+valve+20, LOW);
}

// servo control
void doorClose(int doorNo){
    int pin;
    pin=doorNo+doorNo+21;
    digitalWrite(pin,LOW);
}
void doorOpen(int doorNo){
    int pin;
    pin=doorNo+doorNo+21;
    digitalWrite(pin,HIGH);
}
void allDoorsClose(int doorAmount){
  int pin;
  int doorNo;
  for(doorNo=1;doorNo<=doorAmount;doorNo++){
      pin=doorNo+doorNo+21;
    digitalWrite(pin,LOW);
  }
}
void allDoorsOpen(int doorAmount){
  int pin;
  int doorNo;
  for(doorNo=1;doorNo<=doorAmount;doorNo++){
      pin=doorNo+doorNo+21;
    digitalWrite(pin,HIGH);
  }
}
void rotatorRotate(int timeSpan){
    digitalWrite(33, HIGH);
    delay(timeSpan);
    digitalWrite(33, LOW);
    }
void rotateAndDelay_fold(int delayFold){//More knowledgeable programmers usually avoid the use of delay() for timing of events longer than 10’s of milliseconds
  digitalWrite(33,HIGH);
  delay(5000);
  digitalWrite(33,LOW);
  if (delayFold>1){
    for(int i =1;i<delayFold;i++){
       delay(5000);
     }
  }
}

// gengerate arm index/delay duration/catch trial index for each block
void shuffleFunction(int* shuffledArray, int arraySize){
    int position = 0;
    int iter;
    for (iter=1; iter<16; iter++){
        if ((iter/4) != (iter%4)){
            shuffledArray[position] = iter;
            position++;
        }
        else;
    }
    int temp;
    int index;
    for (iter=arraySize-1; iter>0; iter--){
        index = rand() % (iter+1);
        temp = shuffledArray[index];
        shuffledArray[index] = shuffledArray[iter];
        shuffledArray[iter] = temp;
    }
}
void getSampleAndChoice(int currentTrial, int* sessionArray){

    int* pSample = &mSample;
    int* pChoice = &mChoice;
    int temp = sessionArray[currentTrial-1];

    *pSample = temp/4+1;
    *pChoice = temp%4+1;
}

// test passBound
void sessionFunction_passBound(int sessionNo, int trialNoPerSess, int ITI){
    int currentSession;
    allDoorsClose(5);
    serialSend(SpTrain,1);
    for (currentSession=1; currentSession<=sessionNo; currentSession++){
        serialSend(SpSess,1);
        int armArray[12];
        shuffleFunction(armArray,12);
        int currentTrial = 1;
        while (currentTrial<=trialNoPerSess){
            switch(mState){
                //shuffle
                case PRE_TRIAL :
                    ///close all doors, obtain sample and choice value
                    getSampleAndChoice(currentTrial, armArray);
                    serialSend(SpSample,mSample);
                    serialSend(SpChoice,mChoice);
                    mState = TRIAL;
                    break;
                //main body
                case TRIAL :
                   trialFunction_passBound();
                break;
                //intertrial interval
                case INTERTRIAL :
                    serialSend(SpITI,1);
                    delay(ITI*1000);
                    serialSend(SpITI,0);
                    currentTrial++;
                    mState = PRE_TRIAL;
                    break;

            }
        }
        delay(500);
        serialSend(SpSess,0);
    }
    serialSend(SpTrain,0);
}
void trialFunction_passBound(){
     unsigned int aRead[]={0,analogRead(1),analogRead(2),analogRead(3),analogRead(4),analogRead(5)};
    for (int ii=0;ii<5;ii++){
    serialSend(30+ii,aRead[ii+1]>>2);
    }
    switch(mPhase){
        //open sample door and home doors
        case P0_DOORS_OPEN :
            serialSend(SpTrial,1);
            doorOpen(mSample);
            doorOpen(HOME);
            serialSend(SpSamplePhase,1);
            mPhase = P1_SAMPLE;
            break;
        //detect sample entrance
        case P1_SAMPLE :
        if (aRead[mSample]<passBound[mSample]){
                serialSend(SpSampleEntered,1);
                rewardWater(mSample,luredWater);
                mPhase = P2_HOME_RETURN;
            }
            break;
        //detect home entrance after sampling
        case P2_HOME_RETURN :
            if (aRead[HOME]<passBound[HOME]){
                serialSend(SpSamplePhase,0);
                doorClose(HOME);
                rewardWater(HOME,luredWater);
                mPhase = P3_DELAY;
            }
            break;
        //delay period
        case P3_DELAY :
            ///time duration of delay; rotator ISR; choice door open
            ///rotator
            doorOpen(mSample);///open sample and choice arm in ahead of home doors
            doorOpen(mChoice);
            rotateAndDelay_fold(1);
            doorOpen(HOME);
            serialSend(SpChoicePhase,1);
            mPhase = P4_CHOICE;
            break;
        //detect whether the right choice is made
        case P4_CHOICE :
            if (aRead[mChoice]<passBound[mChoice]){
                serialSend(SpChoiceEntered,1);
                serialSend(SpHit,1);
                mPhase = P5_REWARD;
            }
            else if(aRead[mSample]<passBound[mSample]){
                serialSend(SpChoiceEntered,0);
                serialSend(SpFalseAlarm,1);
                mPhase = P6_SADENDING;
            }
            break;
        //if the correct choice is made
        case P5_REWARD :
            doorClose(mSample);
            rewardWater(mChoice,hitWater);
            mPhase = P7_HAPPYENDING;
            break;
        //if the wrong choice is made
        case P6_SADENDING :
             if (aRead[HOME]<passBound[HOME]){
                doorClose(HOME);
                serialSend(SpChoicePhase,0);
                doorClose(mChoice);
                doorClose(mSample);
                serialSend(SpTrial,0);
                mState = INTERTRIAL;
                mPhase = P0_DOORS_OPEN;
            }
            break;
        //detect home return after choice
        case P7_HAPPYENDING :
            if (analogRead(HOME)<passBound[HOME]){
                doorClose(HOME);
                serialSend(SpChoicePhase,0);
                doorClose(mChoice);
                rewardWater(HOME,luredWater);
                mState = INTERTRIAL;
                mPhase = P0_DOORS_OPEN;
            }
            break;
    }
}

// Forced Alternation (Shaping for DNMS) 
void blockFunction_FA(int blockNum, int trialNumPerBlock,int ifTArm, int trainingPhase,int ITI){
  int trialIndex;
  allDoorsClose(5);
  serialSend(SpTrain,1);// training strating signal
  for (int currentBlock = 0; currentBlock < blockNum ;currentBlock++ ){
    serialSend(SpSess,1);
    int currentTrial = 0;
    while(currentTrial < trialNumPerBlock){
      trialIndex = currentBlock*trialNumPerBlock+currentTrial;
      switch(mState){
         case PRE_TRIAL:
            if(ifTArm == 1){
              mSample = (trialIndex%2 == 0? 1:4);
            }
            else{
              mSample = (trialIndex%2 == 0? 2:3);
            }
            serialSend(SpSample,mSample);
            mState = TRIAL;
         break;
         case TRIAL:
            trialFunction_FA(trainingPhase);
         break;
         case INTERTRIAL:
            serialSend(SpITI,1);
            for (int i=ITI;i>0;i--){ //if delay > 10s, unpridictable error
              delay(10000);//10s
            }
            serialSend(SpITI,0);
            currentTrial++;
            mState = PRE_TRIAL;
         break;
      }
    } 
    serialSend(SpSess,0);// block end   
  }
  serialSend(SpTrain,0);
}
void trialFunction_FA(int trainingPhase){
  //trainingPhase:0-home doors open; 1-home doors close after mouse return
  switch(mPhase){
    case BEFORE_SAMPLE_1:
      serialSend(SpSamplePhase,1);
      doorOpen(mSample);
      doorOpen(HOME);
      mPhase = SAMPLE_1;
    break;
    case SAMPLE_1:
      if (analogRead(mSample)<passBound[mSample]){
        serialSend(SpSamplePhase,2);
        serialSend(SpHit,1);
        rewardWater(mSample,sampleWater);
        mPhase = AFTER_SAMPLE_1;
      }
    break;
    case AFTER_SAMPLE_1:
      if (analogRead(HOME)<passBound[HOME]){
        if (trainingPhase > 0){
          doorClose(HOME);
        }
        serialSend(SpSamplePhase,0);
        rewardWater(HOME,homeWater);
        doorClose(mSample);
        mState = INTERTRIAL;
        mPhase = BEFORE_SAMPLE_1;
      }
    break;
  }
  
}

//Delayed Alternation
void blockFunction_DA(int blockNum, int trialNumPerBlock,int ifTArm, int laserType, int delayDuration,int ifCorrection){
  int trialIndex;
  allDoorsClose(5);
  serialSend(SpTrain,1);// training strating signal
  for (int currentBlock = 0; currentBlock < blockNum ;currentBlock++ ){
    serialSend(SpSess,1);
    int currentTrial = 0;
    while(currentTrial < trialNumPerBlock){
      trialIndex = currentBlock*trialNumPerBlock+currentTrial;
      switch(mState){
         case PRE_TRIAL:  
            if(ifTArm == 1){
              mSample = (trialIndex%2 == 0? 1:4);
            }
            else{
              mSample = (trialIndex%2 == 0? 2:3);
            }
            serialSend(SpSample,mSample);
            mChoice = 5-mSample;
            serialSend(SpChoice,mChoice);
            mDelayDuration = delayDuration; //30s
            serialSend(SpDelay,mDelayDuration);
            serialSend(SpTrialIndex,trialIndex);//for test
            serialSend(SpResult,result);//for test
            if (trialIndex ==0){ // first trial
              mPhase = BEFORE_SAMPLE_1;
            }
            else { //previous trial:hit→choice run;false→sample run
              mPhase = (result == 1?BEFORE_CHOICE_1:BEFORE_SAMPLE_1);
            }
            serialSend(SpTrial,1);
            mState = TRIAL;
         break;
         case TRIAL:
            trialFunction_DA(laserType,ifCorrection);
         break;
         case INTERTRIAL:
            currentTrial++;
            serialSend(SpTrial,1);
            mState = PRE_TRIAL;
         break;
      }
    } 
    serialSend(SpSess,0);// block end   
  }
  serialSend(SpTrain,0);
}
void trialFunction_DA(int laserType,int ifCorrection){
  switch(mPhase){
    case BEFORE_SAMPLE_1:
      serialSend(SpSamplePhase,1);
      doorOpen(mSample);
      laserInTrialPhase(laserArray[laserType][0]);
      rotateAndDelay_fold(mDelayDuration);
      laserOff();
      doorOpen(HOME);
      mPhase = SAMPLE_1;
    break;
    case SAMPLE_1:
      if (analogRead(mSample)<passBound[mSample]){
        serialSend(SpSamplePhase,2);
        rewardWater(mSample,sampleWater);
        mPhase = AFTER_SAMPLE_1;
      }
    break;
    case AFTER_SAMPLE_1:
      if (analogRead(HOME)<passBound[HOME]){
        doorClose(HOME);
        serialSend(SpSamplePhase,0);
        //rewardWater(HOME,homeWater);
        doorClose(mSample);
        mPhase =  BEFORE_CHOICE_1;
      }
    break;
    case BEFORE_CHOICE_1:
      doorOpen(mSample);
      doorOpen(mChoice);
      laserInTrialPhase(laserArray[laserType][0]);
      rotateAndDelay_fold(mDelayDuration);
      laserOff();
      serialSend(SpChoicePhase,1);
      doorOpen(HOME);
      mPhase = CHOICE_1;
    break;
    case CHOICE_1:
      if (analogRead(mSample)<passBound[mSample]){//sample1 arm entered
        serialSend(SpFalseAlarm,1);// 1 for result of SAMPL1-CHOICE1, 2 for result of SAMPL2-CHOICE2  
        result = 0; 
        mPhase = (ifCorrection == 1?  CHOICE_2:AFTER_CHOICE_1);
      }
      else if (analogRead(mChoice)<passBound[mChoice]){//choice1 arm entered
        serialSend(SpHit,1);
        doorClose(mSample);// sample arm closed, as an additional cue and stopping mouse exploring sample arm
        result = 1;
        rewardWater(mChoice, choiceWater);//water reward
        ///rewardWater(mChoice, (trainingPhase<2)?luredWater:hitWater);//water reward
        mPhase = AFTER_CHOICE_1;
      }
    break;
    case CHOICE_2:
      if (analogRead(mSample)<passBound[mSample]){//sample1 arm entered
        mPhase = CHOICE_2;
      }
      else if (analogRead(mChoice)<passBound[mChoice]){//choice1 arm entered
        doorClose(mSample);// sample arm closed, as an additional cue and stopping mouse exploring sample arm
        result = 1;
        rewardWater(mChoice, choiceWater);//water reward
        ///rewardWater(mChoice, (trainingPhase<2)?luredWater:hitWater);//water reward
        mPhase = AFTER_CHOICE_1;
      }
    break;
    case AFTER_CHOICE_1:
      if (analogRead(HOME)<passBound[HOME]){
          doorClose(HOME);
          serialSend(SpChoicePhase,0);//home-returned signal & CHOICE2 phase ended
          allDoorsClose(4);
          if (result == 1){
            rewardWater(HOME,homeWater);//supply water only if hit
          }
          mState = INTERTRIAL;// trial ended;
      }
    break;
  }
}

// DNMS
void blockFunction_DNMS(int blockNum, int trialNumPerBlock,int laserType,int ifTArm, int trainingPhase,int ifVariedDelay,int trainingDay,int delayDuration,int ifCorrection){
  int trialIndex;
  allDoorsClose(5);
  serialSend(SpTrain,1);// training strating signal
  for (int currentBlock = 0; currentBlock < blockNum ;currentBlock++ ){
    serialSend(SpSess,1);
    int currentTrial = 0;
    while(currentTrial < trialNumPerBlock){
      trialIndex = currentBlock*trialNumPerBlock+currentTrial;
      switch(mState){
        case PRE_TRIAL:
            if (ifTArm == 1){
              mSample = TTarmArray[trainingDay][trialIndex];
              mChoice = 5-mSample;
            }
            else {
              mSample = YYarmArray[trainingDay][trialIndex];
              mChoice = 5-mSample;
            }
            if (ifVariedDelay == 1){
              mDelayDuration = delayArray[trainingDay][trialIndex][0];
            }
            else {
              mDelayDuration = delayDuration; //30s
            }
            serialSend(SpSample,mSample);
            serialSend(SpChoice,mChoice); 
            serialSend(SpDelay,mDelayDuration);
            mState = TRIAL;
        break;
        case TRIAL:
            trialFunction_DNMS(laserType,trainingPhase);
        break;
        case INTERTRIAL:
            serialSend(SpITI,1);
            for (int i=0;i<delayDuration;i++){
              delay(10000);//ITI 10s*6, 60s
            }
            //delay(mDelayDuration*2000);
            serialSend(SpITI,0);
            if (result == 1||ifCorrection == 0){// repeat same trial if false
              currentTrial++;
            } 
            mState = PRE_TRIAL;
        break;
      }
    }
    serialSend(SpSess,0);// block end 
  }
  serialSend(SpTrain,0);
}
void trialFunction_DNMS(int laserType,int trainingPhase){
  switch(mPhase){
    case BEFORE_SAMPLE_1:
      serialSend(SpSamplePhase,1);
      doorOpen(mSample);
      doorOpen(HOME);
      mPhase = SAMPLE_1;
    break;
    case SAMPLE_1:
      if (analogRead(mSample)<passBound[mSample]){
        serialSend(SpSamplePhase,2);
        rewardWater(mSample,sampleWater);
        mPhase = AFTER_SAMPLE_1;
      }
    break;
    case AFTER_SAMPLE_1:
      if (analogRead(HOME)<passBound[HOME]){
        if (trainingPhase > 0){
          doorClose(HOME);
        }
        serialSend(SpSamplePhase,0);
        rewardWater(HOME,(trainingPhase<2?homeWater:0));
        doorClose(mSample);
        mPhase =  DELAY_1;
      }
    break;
    case DELAY_1: 
      doorOpen(mChoice);
      if (trainingPhase >1){
        doorOpen(mSample);
      }  
      if(trainingPhase>0){
        laserInTrialPhase(laserArray[laserType][0]);
        rotateAndDelay_fold(mDelayDuration);
        laserOff();
      }
      mPhase = BEFORE_CHOICE_1; 
    break;
    case BEFORE_CHOICE_1:
      serialSend(SpChoicePhase,1);
      doorOpen(HOME);
      mPhase = CHOICE_1;
    break;
    case CHOICE_1:
      if (analogRead(mSample)<passBound[mSample]){//sample1 arm entered
        serialSend(SpFalseAlarm,1);// 1 for result of SAMPL1-CHOICE1, 2 for result of SAMPL2-CHOICE2 
        if (trainingPhase<2){//shaping       
          doorClose(mChoice);
        }
        result = ((trainingPhase>1)?0:1); 
        rewardWater(mSample, (trainingPhase<2)?choiceWater:0);
        mPhase = AFTER_CHOICE_1;
      }
      else if (analogRead(mChoice)<passBound[mChoice]){//choice1 arm entered
        serialSend(SpHit,1);
        doorClose(mSample);// sample arm closed, as an additional cue and stopping mouse exploring sample arm
        result = 1;
        rewardWater(mChoice, choiceWater);//water reward
        ///rewardWater(mChoice, (trainingPhase<2)?luredWater:hitWater);//water reward
        mPhase = AFTER_CHOICE_1;
      }
    break;
    case AFTER_CHOICE_1:
      if (analogRead(HOME)<passBound[HOME]){
         if (trainingPhase > 0){
          doorClose(HOME);
         }
          serialSend(SpChoicePhase,0);//home-returned signal & CHOICE2 phase ended
          allDoorsClose(4);
          if (result == 1){
            rewardWater(HOME,homeWater);//supply water only if hit
          }
          mState = INTERTRIAL;// trial ended
          mPhase = BEFORE_SAMPLE_1;
      }
    break;
  }
}

void unicornFunction(int n){
	switch(n){   
    case 1500:{//DNMS shaping with home doors open
    //blockFunction_DNMS(int blockNum, int trialNumPerBlock,int laserType,int ifTArm, int trainingPhase,int ifVariedDelay,int trainingDay)
        int blockNum = getFuncNumber(1);
        int ifTArm = getFuncNumber(1);
        int trainingDay = getFuncNumber(1);
       blockFunction_DNMS(blockNum,12,0,ifTArm,0,0,trainingDay,0,0);
    }
    break;
    case 1510:{//DNMS shaping
        int blockNum = getFuncNumber(1);
        int ifTArm = getFuncNumber(1);
        int trainingDay = getFuncNumber(1);
        int delayDuration = getFuncNumber(1);
        blockFunction_DNMS(blockNum,12,0,ifTArm,1,0,trainingDay,delayDuration,0);
    }
    break;
    case 1520:{//DNMS training (with correctionTrial)
        int blockNum = getFuncNumber(1);
        int ifTArm = getFuncNumber(1);
        int laserType = getFuncNumber(1);
        int trainingDay = getFuncNumber(1);
        int delayDuration = getFuncNumber(1);
        blockFunction_DNMS(blockNum,12,laserType,ifTArm,2,0,trainingDay,delayDuration,1);
    }
    case 1521:{//DNMS training (pure DNMS task)
        int blockNum = getFuncNumber(1);
        int ifTArm = getFuncNumber(1);
        int laserType = getFuncNumber(1);
        int trainingDay = getFuncNumber(1);
        int delayDuration = getFuncNumber(2);
        blockFunction_DNMS(blockNum,12,laserType,ifTArm,2,0,trainingDay,delayDuration,0);
    }
    break;
    case 1522:{//DNMS training (pure DNMS task),varied delay
        int blockNum = getFuncNumber(1);
        int ifTArm = getFuncNumber(1);
        int laserType = getFuncNumber(1);
        int trainingDay = getFuncNumber(1);
        blockFunction_DNMS(blockNum,12,laserType,ifTArm,2,1,trainingDay,0,0);
    }
    break;

    //14** test code
    case 1400:
      allDoorsOpen(5);
    break;
    case 1401://test servos
        allDoorsOpen(6);
        delay(3000);
        allDoorsClose(6);
    break;
    case 1402:{//test sigle servo
       int n=getFuncNumber(1);
       int armID=getFuncNumber(1);
       switch(n){
        case 1:
        doorOpen(armID);
        break;
        case 2:
        doorClose(armID);
        break;
       }
     }
    break;
    case 1410:{//test infrared
    int n=getFuncNumber(1);
    int passBound = getFuncNumber(2);
    while(1){
      int threshold = passBound*10;
      if(analogRead(n)<threshold){
        digitalWrite(13,HIGH);
      }
      else if (analogRead(n)>threshold){
        digitalWrite(13,LOW);
      }
    }
    }
    break;    
    case 1411://test passBound
        sessionFunction_passBound(4,12,1);
    break;
    
    case 1420:{ // test water volume
      int waterVolume = getFuncNumber(1);
      for (int armIndex = 1; armIndex <6;armIndex ++){
        for (int iter = 0; iter<100; iter++){
          valveOn(armIndex);
          delay(waterVolume == 1?hitWater:luredWater);
          valveOff(armIndex);
          delay(100); 
        }
      }
    }  
    break; 
    case 1421://test pumps
        int iter;
        for(iter=1;iter<=5;iter++){
           valveOn(iter);
           delay(3000);
           valveOff(iter);
          }
    break;
    case 1422:{ // test water volume
       int waterVolume = getFuncNumber(1);
       int armIndex=getFuncNumber(1);
       for (int iter = 0; iter<100; iter++){
        valveOn(armIndex);
        delay(waterVolume == 1?hitWater:luredWater);
        valveOff(armIndex);
        delay(100); 
       }
    }
    break;    
    case 1423:{ // test water pump (drop by drop
       int waterVolume = getFuncNumber(2);
       int armIndex=getFuncNumber(1);
       for (int iter = 0; iter<100; iter++){
        valveOn(armIndex);
        delay(waterVolume*10);
        valveOff(armIndex);
        delay(1000); 
       }
    }
    break;
   
    case 1430:{//test laser
     int laserType = getFuncNumber(1);
       for (int phase = 0; phase<13; phase++){
        laserInTrialPhase(laserArray[laserType][phase]);
        delay(2000);
       }
    }
    break;
  }
}


