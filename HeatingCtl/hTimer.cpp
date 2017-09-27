
#include "hTimer.h"
  
void hTimer::SetCounter(unsigned long hCount){
    _target = hCount;
    counter = millis();

    Serial.print(F("INFO: hTimer::SetCounter Timer Started for "));
    Serial.print(hCount);
    Serial.println(F("mS"));
}
    
bool hTimer::timeup() {return  (millis() - counter > _target);}

bool hTimer::timeup(unsigned long thisTarget) {return (millis() - counter > thisTarget);}


 


