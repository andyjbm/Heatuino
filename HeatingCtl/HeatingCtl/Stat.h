
#ifndef Stat_h
#define Stat_h

#include <Arduino.h>
#include <Bounce2.h>
#include "hTimer.h"

enum StatStates {DEMAND, NODEMAND, DELAYINGDEMAND};

//enum StatChangeType {OFF2ON, ON2OFF, NONE};

class Stat {

  public: 

     //Constructor
     Stat(String StatName, unsigned long Delay, int StatPin);

     String _StatName;
     StatStates GetStatState();
     StatStates GetStatSecretState();

     String ReturnStatState(StatStates State); 

  private:
     void ReadStat();
     StatStates GetStateFromSwitch();
     Bounce Stat_B = Bounce();
     hTimer StatDelayTimer;
     unsigned long _Delay;
};

#endif
