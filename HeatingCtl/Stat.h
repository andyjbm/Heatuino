
#ifndef Stat_h
#define Stat_h

#include <Arduino.h>
#include <Bounce2.h>

enum StatStates {DEMAND, NODEMAND};
//enum StatChangeType {OFF2ON, ON2OFF, NONE};

class Stat {

  public: 

     //Constructor
     Stat(String StatName, int StatPin);

     String _StatName;
     StatStates GetStatState();

     String ReturnStatState(bool State); 

  private:
     Bounce Stat_B = Bounce();
};

#endif
