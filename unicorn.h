
#ifndef UNICORN0805_H
#define  UNICORN0805_H
#include <TimerOne.h>
#include <Servo.h>

#define HOME 5


#define PRE_TRIAL 0
#define TRIAL 1
#define INTERTRIAL 2
#define SAMPLE_RUN 4
#define CHOICE_RUN 7

#define P0_DOORS_OPEN 0
#define P1_SAMPLE 1
#define P2_HOME_RETURN 2
#define P3_DELAY 3
#define P4_CHOICE 4
#define P5_REWARD 5
#define P6_ERROR_CORRECTION 6
#define P7_ENDING 7
#define P6_SADENDING 6
#define P7_HAPPYENDING 7
#define P8_DISTRACTOR 8
#define P9_HOME_RETURN 9
#define P10_CORRECT 10
#define LASER_OFF 0
#define LASER_WHOLE_TRIAL 1
#define LASER_SAMPLE 2
#define LASER_DELAY 3
#define LASER_CHOICE 4
#define LASER_ITI 5
#define LASER_TYPE_NO 9
#define TRIAL_PHASE_NO 13

#define BEFORE_SAMPLE_1  0
#define SAMPLE_1         1
#define AFTER_SAMPLE_1   2
#define BEFORE_SAMPLE_2  3
#define SAMPLE_2         4
#define AFTER_SAMPLE_2   5
#define DELAY            6
#define BEFORE_CHOICE_1  7
#define CHOICE_1         8
#define AFTER_CHOICE_1   9
#define BEFORE_CHOICE_2  10
#define CHOICE_2         11
#define AFTER_CHOICE_2   12
#define DELAY_1          13
#define DELAY_2          14
#define DELAY_3          15


  #define SpResult    0  //  SpLick//      !PORTDbits.RD8
  #define SpCome   1    //Keypad #[ZX] // Head fixed, always come
  #define SpTrialIndex  2   // SpRun // lick2    //!PORTDbits.RD9
  #define SpCome2  3    // Head fixed, always come
  // the second word is for performance, false alarm,
  #define SpFalseAlarm   4  // false alarm //1-1st DNMS, 2 - 2nd DNMS (nested dual-task)
  #define SpCorrectRejection     5  // correct rejection
  #define SpMiss     6  // miss
  #define SpHit    7  // hit //1-1st DNMS, 2 - 2nd DNMS (nested dual-task)
  // the third word is for the first set of valves

  #define SpDelay   9         
  #define SpDelay2  10 
  #define SpDelay3  11      
  #define SpDelay4  64

  // the fourth word is for the second set of vales
  #define SpSample  12 ///1 2 3 4 till n
  #define SpChoice  13 ///1 2 3 4 till n
  #define SpDistractor  14
  #define SpCatchTrial  15 ///1 for catch trial, 0 for blank Trial
  #define SpTrialLaser  16 
  #define SpPairedArm  17
  #define SpNonPairedArm  18
  #define SpOther 19


  //press key, s1s1 through s5s5
  #define SpSamplePhase   20 //1 start 0 end//S1S1
  #define SpChoicePhase   21 //1 start 0 end//S1S2
  #define SpTrial   22 //1 start 0 end     //S1S3
  #define SpHome   23 //1 out 0 in         //S1S4
  #define SpDistractorPhase   24 //1 start 0 end //S1S5
  #define SpChoiceArm   25 //1 out 0 in    //S2S1
  #define SpSamplePhase2   26              //S2S2
  #define SpChoicePhase2   27              //S2S3
  #define SpSample2   28                   //S2S4
  #define SpChoice2  29                    //S2S5
  #define SpSampleEntered   30             //S3S1
  #define SpChoiceEntered   31             //S3S2

  #define Sptrialtype     58
  #define SpITI           59  // 1 start 0 end
  #define SpCatch         60  //1 for catch trial, 0 for blank trial
  #define SpSess          61  // 1 start 0 end
  #define SpTrain         62  // 1 start 0 end
  #define SpLaser         65




  //extern unsigned int TimerCounterI;
  extern unsigned int Hit, miss, FalseAlarm, correctRejection, CorrectRatio;
  extern int currentMiss;
  

  void Valve_ON(int valve);
  void Valve_OFF(int valve);
  void phaseState(int aimToArm, int firstHalf);
  void laserON();
  void laserOFF();
  void laserInTrialPhase(int laserOnOrOff);
  int getFuncNumber(int targetDigits);
  void serialSend(int type, int value);
  void unicornFunction(int n);
  void allDoorsClose(int doorAmount);
  void allDoorsOpen(int doorAmount);
  void callReset();
  void valveOn(int valve);
  void valveOff(int valve);
  void rewardWater(int valve, int waterTime);
  void doorClose(int doorNo);
  void doorOpen(int doorNo);
  void rotatorRotate(int timeSpan);
  void rotateAndDelay_fold(int delayFold);
  void shuffleFunction(int* shuffledArray, int arraySize);
  void sessionFunction_passBound(int sessionNo, int trialNoPerSess, int ITI);
  void trialFunction_passBound();
  void blockFunction_FA(int blockNum, int trialNumPerBlock,int ifTArm, int trainingPhase,int ITI);
  void trialFunction_FA(int trainingPhase);
  void blockFunction_DA(int blockNum, int trialNumPerBlock,int ifTArm, int laserType,int ifCorrection);
  void trialFunction_DA(int laserType,int ifCorrection);
  void blockFunction_DNMS(int blockNum, int trialNumPerBlock,int laserType,int ifTArm, int trainingPhase,int ifVariedDelay,int trainingDay,int delayDuration,int ifCorrection);
  void trialFunction_DNMS(int laserType,int trainingPhase);
#endif

