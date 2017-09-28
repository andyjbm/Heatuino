


/*
 * 
 * Version 0.4
 * 
 * Mod to use Prerun for the 1st minute of CH demand
 * so that wnen the HR80s are cycling 1 minute in 10
 * we use heat from the boiler without firing.
 * 
 */

#include "Stat.h"
#include "TwoPort.h"
#include "hTimer.h"

#define PRERUN_CH   70000 // 1 minute 10 seconds prerun for CH to determine if we are doing 1 min DEMAND in 10 cycling.
#define PRERUN_HW   5000   // 5 seconds prerun.

#define STATDELAY_CH 0     //Delay was 1 minute 30 Sec. To defeat stupid Honeywell cycling. Now handled by extended prerun for CH
#define STATDELAY_HW 0     //

#define OVERRUN 300000     // 5 minutes overrun, 

//Inputs
#define Motor1OpenPin     2   //PD2
#define Motor1ClosedPin   3   //PD3
#define Motor2OpenPin     4   //PD4
#define Motor2ClosedPin   5   //PD5

#define TankStatPin       6   //PD6
#define RoomStatPin       7   //PD7

#define PressureSwitchPin 8   //PB0
#define SparePin          9   //PB1

//Analogue - Reserved for future HM200D TW connection maybe?
#define Analog0     A0   //ADC0
#define Analog1     A1   //ADC1

//Outputs - These pins were chosen for outputs because they remain in an off state during boot.
#define Motor1Pin   A2   //ADC2
#define Motor2Pin   A3   //ADC3
#define PumpPin     A4   //ADC4
#define BoilerPin   A5   //ADC5


//Globals
Stat TankStat("TankStat", STATDELAY_HW, TankStatPin);
Stat RoomStat("RoomStat", STATDELAY_CH, RoomStatPin); 

TwoPort PortHW("HW", Motor1Pin, Motor1OpenPin, Motor1ClosedPin);
TwoPort PortCH("CH", Motor2Pin, Motor2OpenPin, Motor2ClosedPin);

hTimer htTimer;
hTimer chTimer;

enum SystemStates {SYSTEM_OFF, SYSTEM_PRERUN, SYSTEM_ON, SYSTEM_OVERRUN, SYSTEM_IDLE};

SystemStates SystemState;

StatStates RoomStat_StateAtStartOfOverrun;


String ReturnSystemState(SystemStates State){
  switch (State){
    case SYSTEM_OFF:     return "SYSTEM_OFF";
    case SYSTEM_IDLE:    return "SYSTEM_IDLE";
    case SYSTEM_PRERUN:  return "SYSTEM_PRERUN";
    case SYSTEM_OVERRUN: return "SYSTEM_OVERRUN";
    case SYSTEM_ON:      return "SYSTEM_ON";
  }
}

void MoveTo(SystemStates NewState){

  Serial.print(F("INFO: Main::MoveTo "));
  Serial.print(F("System changing from "));
  Serial.print(ReturnSystemState(SystemState));
  Serial.print(F(" to "));
  Serial.println(ReturnSystemState(NewState));

  switch(NewState){
    case SYSTEM_PRERUN:
       digitalWrite(PumpPin, LOW); //Pump ON
       SystemState = SYSTEM_PRERUN;
       break;
       
    case SYSTEM_ON:
       digitalWrite(BoilerPin, LOW); //Boiler ON
       SystemState = SYSTEM_ON;
       break;
    
    case SYSTEM_OVERRUN:
       digitalWrite(BoilerPin, HIGH); //Boiler OFF
       SystemState = SYSTEM_OVERRUN;
       htTimer.SetCounter(OVERRUN); // 5 Minutes Overrun
       RoomStat_StateAtStartOfOverrun = RoomStat.GetStatState();
       break;

    case SYSTEM_IDLE:
       digitalWrite(PumpPin, HIGH); //Pump OFF
       SystemState = SYSTEM_IDLE;
       htTimer.SetCounter(300000); // 1 minute
       break;

    case SYSTEM_OFF:
      //Just make sure they didn't get opened and abandoned when in OVERRUN or IDLE.
      PortHW.Request(CLOSE);
      PortCH.Request(CLOSE);
      SystemState = SYSTEM_OFF;
      break;
  }
}
    
void checkSystem(){

   switch (SystemState){
    case SYSTEM_OFF:
    
       if (RoomStat.GetStatState() == DEMAND) 
         if (PortCH.Request(OPEN)){
           MoveTo(SYSTEM_PRERUN);
           htTimer.SetCounter( PRERUN_CH ); // 1:10 for CH Prerun
           break;
         }
       if (TankStat.GetStatState() == DEMAND)
          if (PortHW.Request(OPEN)){
            MoveTo(SYSTEM_PRERUN);
            htTimer.SetCounter( PRERUN_HW ); 
            break;
          }
       break;   
    
    case SYSTEM_PRERUN:
       if (htTimer.timeup()) MoveTo(SYSTEM_ON);
       if (PortCH.RequestedState == CLOSE and RoomStat.GetStatState() == DEMAND) PortCH.Request(OPEN);
       if (PortHW.RequestedState == CLOSE and TankStat.GetStatState() == DEMAND) PortHW.Request(OPEN);

       // In the case where we are on CH demand and during the longer prerun wait the TankStat opens, we check to see if we have ALREADY waited long enough for TankStat Prerun.
       if (PortHW.RequestedState == OPEN and htTimer.timeup( PRERUN_HW )) MoveTo(SYSTEM_ON);
       break;
       
    case SYSTEM_ON:

       // If the other port is closed and it's stat demands heat, then open it. So both ports will be open.
       if (PortCH.RequestedState == CLOSE and RoomStat.GetStatState() == DEMAND) PortCH.Request(OPEN);
       if (PortHW.RequestedState == CLOSE and TankStat.GetStatState() == DEMAND) PortHW.Request(OPEN);

       //CH wants to switch off?
       if (RoomStat.GetStatState() == NODEMAND and PortCH.RequestedState == OPEN) //So it's open and we want to close it.
           
           // We can only close CH if HW is open and staying open. Check the request 1st.
           if (PortHW.RequestedState == OPEN) {// Yes request is open so it will be opening if its not open already...
              // Wait for HW to BE PHYSICALLY open and then close CH once we know HW is open.
              if (PortHW.PortState == OPENED) PortCH.Request(CLOSE); //Leave this to get on with it.
           } else { //
              // So HW is off and CH wants Off. 
              MoveTo(SYSTEM_OVERRUN);
              break;
           }
       // Same as above but the other way round.
       if (TankStat.GetStatState() == NODEMAND and PortHW.RequestedState == OPEN)
          // HW Wants to close
          if (PortCH.RequestedState == OPEN) { // CH will be open or opening.
             //Check CH has opened.
             if (PortCH.PortState == OPENED) PortHW.Request(CLOSE); // ...and leave this to get on with it.
          } else {
            // So CH is off and HW wants off.
            MoveTo(SYSTEM_OVERRUN);
            break;   
          }
       break;

    case SYSTEM_OVERRUN:

       // Look for CH DEMAND again...
       if (RoomStat.GetStatState() == DEMAND) {
         if (RoomStat_StateAtStartOfOverrun == NODEMAND) {
            // RoomStat just asked for demand so start a prerun timer (to avoid 1 in 10 cycling) and time this inside SYSTEM_OVERRUN
            chTimer.SetCounter(PRERUN_CH);
            RoomStat_StateAtStartOfOverrun = DEMAND;
         }   
         if (PortCH.Request(OPEN)) {
            if (TankStat.GetStatState() == NODEMAND) 
               // Close HW before turning the boilder back on. 
               // If there is HW demand before we get here we would have already gone back to SYSTEM_ON from code further down.
               if (PortHW.Request(CLOSE)) {
                  //Wait for PRERUN to expire before firing the boiler. This allows the OVERRUN to be the source of heat for 1 in 10 cycling.
                  //Note that even if the OVERRUN timer expires before this PRERUN expires we will stay here because RoomStat is in DEMAND.
                  if (chTimer.timeup()) MoveTo(SYSTEM_ON);
               }
         }
       } else { //No CH Demand so carry on with Overrun
        
          // Open the HW port and when that's done, close the CH port.
          if (PortHW.Request(OPEN)) PortCH.Request(CLOSE);
       }

       //Note a DEMAND from TankStat moves to SYSTEM_ON without a PRERUN as there will be no 1 in 10 cycling from a HW request!
       if (TankStat.GetStatState() == DEMAND) if (PortHW.Request(OPEN)) MoveTo(SYSTEM_ON); // This is the state we will probably be in here so this should move straight to SYSTEM_ON.

       //if there is no further demand, move to IDLE when the OVERRUN time expires.
       if (RoomStat.GetStatState() == NODEMAND and TankStat.GetStatState() == NODEMAND and htTimer.timeup()) MoveTo(SYSTEM_IDLE);
       break;
    
    case SYSTEM_IDLE:
       
       if (RoomStat.GetStatState() == DEMAND) {
         //There is demand for CH so close HW while CH opens...
         if (TankStat.GetStatState() == NODEMAND) PortHW.Request(CLOSE);
         if (PortCH.Request(OPEN)) MoveTo(SYSTEM_PRERUN);  
       }
       
       if (TankStat.GetStatState() == DEMAND) {
         //There is demand for HW so close CH while HW opens...
         if (RoomStat.GetStatState() == NODEMAND) PortCH.Request(CLOSE); 
          if (PortHW.Request(OPEN)) MoveTo(SYSTEM_PRERUN);
       }

       //Otherwise we're all done.   
       if (RoomStat.GetStatState() == NODEMAND and TankStat.GetStatState() == NODEMAND and htTimer.timeup()) MoveTo(SYSTEM_OFF);   
   } 
}


