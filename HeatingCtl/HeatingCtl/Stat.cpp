#include "Stat.h"
#include <Bounce2.h>
#include <Arduino.h>
#include "hTimer.h"

Stat::Stat(String StatName, unsigned long Delay, int StatPin) //Constructor
{
   _StatName = StatName;
   _Delay = Delay;
   //ChangePending = NONE;
   pinMode(StatPin, INPUT_PULLUP);
   Stat_B.attach( StatPin );
   Stat_B.interval(5);
}


void Stat::ReadStat() {
  if (Stat_B.update()) {
    Serial.print(F("INFO: Stat::GetStatState "));
    Serial.print(_StatName);
    Serial.print(F(" changed to state "));
    Serial.println(ReturnStatState(GetStateFromSwitch()));
    if (GetStateFromSwitch() == DEMAND and _Delay > 0) { // We just changed to demand so start the delay timer.
      Serial.print(F("INFO: Stat Delay Timer Started: "));
      Serial.print(_Delay);
      Serial.println(F(" mS"));
      StatDelayTimer.SetCounter(_Delay);
    }
  }
}
StatStates Stat::GetStateFromSwitch() {
  switch (Stat_B.read()){
     case HIGH: return NODEMAND; // This is off
     case LOW:  return DEMAND;   // This is ON
  }
}


//This returns DEMAND or NODEMAND. 
//The DelayTimer result is hidden and a delay in progress returns NODEMAND.

StatStates Stat::GetStatState(){
  ReadStat();
  switch (Stat_B.read()){
     case HIGH: // This is off
        return NODEMAND;
        
     case LOW:  // This is ON
        if ((_Delay == 0) or (_Delay > 0 and StatDelayTimer.timeup())) 
          return DEMAND;
        else 
          return NODEMAND;
  }
}


//This returns DEMAND, NODEMAND or DELAYINGDEMAND.

StatStates Stat::GetStatSecretState() {
   ReadStat();
   switch (Stat_B.read()){
     case HIGH: // This is off
        return NODEMAND;
        
     case LOW:  // This is ON
        if (_Delay == 0)          return DEMAND;
        if (StatDelayTimer.timeup())  return DEMAND; 
        return DELAYINGDEMAND;
   }
}

String Stat::ReturnStatState(StatStates State){
  switch(State){
    case DEMAND         : return "DEMAND";
    case NODEMAND       : return "NODEMAND";
    case DELAYINGDEMAND : return "DELAYINGDEMAND";
  }
}


