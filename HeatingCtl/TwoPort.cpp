
#include <Arduino.h>
#include "TwoPort.h"
#include <Bounce2.h>

TwoPort::TwoPort(String _PortName, int MotorPin, int OpenPin, int ClosedPin){

  PortName = _PortName;
  
//  Serial.print("INFO: TwoPort::TwoPort ");
//  Serial.print(PortName);
//  Serial.println(" TwoPort Valve created.");

  //Doesn't work in a constructor?

  
  PortState = NOTKNOWN;
  
  _MotorPin = MotorPin;

  digitalWrite(MotorPin, HIGH);
  pinMode(MotorPin, OUTPUT);
  
  pinMode(OpenPin, INPUT_PULLUP);
  pinMode(ClosedPin, INPUT_PULLUP);

  MotorOpen_B.attach( OpenPin );
  MotorOpen_B.interval(5);

  MotorClosed_B.attach( ClosedPin );
  MotorClosed_B.interval(5);

}

String TwoPort::ReturnPortState(PortStates State){
  switch(State){
    case OPENED:  return "OPENED";
    case OPENING: return "OPENING";
    case CLOSED:  return "CLOSED";
    case CLOSING: return "CLOSING";
  }
}

String TwoPort::ReturnRequestState(RequestStates State){
  switch(State){
    case OPEN:  return "OPEN";
    case CLOSE: return "CLOSE";
  }
}


bool TwoPort::InTransition() {      
   return ((PortState == OPENING) or (PortState == CLOSING) or (PortState == NOTKNOWN));
}   

bool TwoPort::Request(RequestStates _RequestedState){
  
  if (RequestedState != _RequestedState)
            {
               Serial.print(F("INFO: TwoPort::Request "));
               Serial.print(PortName);
               Serial.print(F(" RequestStatus changed from "));
               Serial.print(ReturnRequestState(RequestedState));
               Serial.print(F(" to "));
               Serial.print(ReturnRequestState(_RequestedState));
               Serial.print(F(". Current Port State is "));
               Serial.print(ReturnPortState(PortState));

               if (_RequestedState == OPEN) 
                 {
                   if (PortState ==  OPENED) 
                     {
                       digitalWrite(_MotorPin, HIGH);
                       Serial.println(F(" - Motor OFF."));
                     } 
                   else 
                     {
                       digitalWrite(_MotorPin, LOW);           
                       Serial.println(F(" - Motor ON."));
                     }
                 }
               else // CLOSE
                 {
                   if (PortState == CLOSED) 
                     {
                       digitalWrite(_MotorPin, HIGH);
                       Serial.println(F(" - Motor OFF."));
                     } 
                   else 
                     {
                       digitalWrite(_MotorPin, LOW);
                       Serial.println(F(" - Motor ON."));
                     }
                 }
            }
            
  RequestedState = _RequestedState;
  
  if (RequestedState == OPEN) 
     {
       if (PortState ==  OPENED) return true; else return false;
     }
  else // CLOSE
     {
        if (PortState == CLOSED) return true; else return false;
     }
}

void TwoPort::checkPorts() {
   switch (PortState) { // Current State
    
    case OPENED:
      if (MotorOpen_B.update()) //It Changed
         if (MotorOpen_B.read() == HIGH){ //Check the actual value. Note LOW is switch closed ie Port is in open position.
            //We've left the open position and are now moving towards closing (but our destination may change to OPEN before we reach the CLOSED position!)
            PortState = CLOSING;
            Serial.print(F("INFO: TwoPort::checkPorts "));
            Serial.print(PortName);
            Serial.println(F(" PortOpen switch changed from OPEN to NOTOPEN. PortState CLOSING."));
            break;
         }
      break;
      
    case CLOSED:
      if (MotorClosed_B.update())
         if (MotorClosed_B.read() == HIGH){
            //Opening - ditto above but for opening...
            PortState = OPENING;
            Serial.print(F("INFO: TwoPort::checkPorts "));
            Serial.print(PortName);
            Serial.println(F(" PortClosed switch changed from CLOSED to NOTCLOSED. PortState OPENING."));
            break;
         }
      break;
      
    case OPENING:
       if (MotorOpen_B.update()) 
          if (MotorOpen_B.read() == LOW){
             PortState = OPENED;
             Serial.print(F("INFO: TwoPort::checkPorts "));
             Serial.print(PortName);
             Serial.println(F(" PortOpen switch changed from NOTOPEN to OPEN. PortState OPENED."));
             //Opened, stop the motor if this was our destination.
             if (RequestedState == OPEN) {
                digitalWrite(_MotorPin, HIGH);
                Serial.print(F("INFO: TwoPort::checkPorts "));
                Serial.print(PortName);
                Serial.println(F(" Port reached destination OPEN. Motor switched OFF."));
             }
             break;
          }
       break;
       
    case CLOSING:
       if (MotorClosed_B.update())
          if (MotorClosed_B.read() == LOW){
             PortState = CLOSED;
             Serial.print(F("INFO: TwoPort::checkPorts "));
             Serial.print(PortName);
             Serial.println(F(" PortClosed switch changed from NOTCLOSED to CLOSED. PortState CLOSED."));
             //Closed, Stop the motor if this was our destination.
             if (RequestedState == CLOSE) {
                digitalWrite(_MotorPin, HIGH);
                Serial.print(F("INFO: TwoPort::checkPorts "));
                Serial.print(PortName);
                Serial.println(F(" Port reached destination CLOSED. Motor switched OFF."));
             }
             break;
          }
       break;
       
    case NOTKNOWN: //for Power up where we don't know where the valves are.
       
       RequestedState = CLOSE;
       MotorClosed_B.update();
       MotorOpen_B.update();

       if (MotorOpen_B.read() == LOW) PortState == OPENED;
       if (MotorClosed_B.read() == HIGH) { 
          //Not Closed so close it.
          digitalWrite(_MotorPin, LOW);
          Serial.print(F("INFO: TwoPort::checkPorts "));
          Serial.print(PortName);
          Serial.println(F(" PortState NOTKNOWN - CLOSING - Motor switched ON."));
          PortState = CLOSING; //We could actually be OPENING but we don't know and we want to close so we'll say we're closing.
          break;      
       } else {
          //Closed
          PortState = CLOSED;
          Serial.print(F("INFO: TwoPort::checkPorts "));
          Serial.print(PortName);
          Serial.println(F(" PortState KNOWN. Port CLOSED. Motor switched OFF."));
          digitalWrite(_MotorPin, HIGH); // Really Needed???
       }
       break;
  }
}

