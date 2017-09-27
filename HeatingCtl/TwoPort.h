
#ifndef TwoPort_h
#define TwoPort_h

#include "Arduino.h"
#include <Bounce2.h>

enum PortStates { OPENING, CLOSING, OPENED, CLOSED, NOTKNOWN };
enum RequestStates {OPEN, CLOSE};

class TwoPort {

  public:

     PortStates PortState;
     RequestStates RequestedState;
     String PortName;

     TwoPort(String _PortName, int MotorPin, int OpenPin, int ClosedPin);

     bool Request(RequestStates _RequestState);
     
     bool InTransition();

     void checkPorts();

     String ReturnPortState(PortStates State);    
     String ReturnRequestState(RequestStates State);
  private:

     Bounce MotorOpen_B  = Bounce();
     Bounce MotorClosed_B  = Bounce();
     int _MotorPin;
     
};


#endif
