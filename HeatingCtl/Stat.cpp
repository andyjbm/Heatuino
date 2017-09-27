#include "Stat.h"
#include <Bounce2.h>
#include <Arduino.h>


Stat::Stat(String StatName, int StatPin) //Constructor
{
   _StatName = StatName;
   //ChangePending = NONE;
   pinMode(StatPin, INPUT_PULLUP);
   Stat_B.attach( StatPin );
   Stat_B.interval(5);
}


StatStates Stat::GetStatState() {
  if (Stat_B.update()) {
    Serial.print(F("INFO: Stat::GetStatState "));
    Serial.print(_StatName);
    Serial.print(F(" changed to state "));
    Serial.println(ReturnStatState(Stat_B.read()));
  }
  switch (Stat_B.read()){
     case HIGH: // This is off
        return NODEMAND;
        
     case LOW:  // This is ON
        return DEMAND;
  }     
}

String Stat::ReturnStatState(bool State){
  switch(State){
    case LOW:  return "DEMAND";
    case HIGH: return "NODEMAND";
  }
}


