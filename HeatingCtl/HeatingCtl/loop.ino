
#include "Stat.h"

unsigned long LoopCounter;

void loop() {
   
  PortCH.checkPorts();
  PortHW.checkPorts();
  
  checkSystem();
  
  delay(500);
  
  //if (LoopCounter < (millis() / 5000) ) {
  //  LoopCounter += 1;
  //  Serial.print("INFO: SystemState ");
  //  Serial.println(ReturnSystemState(SystemState));
  //
  //  Serial.print("INFO: Tank Stat ");
  //  Serial.println(TankStat.ReturnStatState(TankStat.GetStatState()));
  //  
  //  Serial.print("INFO: Room Stat ");
  //  Serial.println(RoomStat.ReturnStatState(RoomStat.GetStatState()));
  //  
  //}
  
  //Post State Data to EMONCMS
  if (LoopCounter < (millis() / 10000) ) {
    LoopCounter += 1;
    Serial.print(F("EMONCMS: 11 "));
    Serial.print(SystemState);
    Serial.print(F(" "));
    Serial.print(TankStat.GetStatSecretState());
    Serial.print(F(" "));
    Serial.println(RoomStat.GetStatSecretState());
  }

}


