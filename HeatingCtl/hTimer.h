
#ifndef hTimer_h
#define hTimer_h

#include <Arduino.h>

class hTimer {

  private: 

    unsigned long counter;
    unsigned long _target;
  
  public:
    void SetCounter(unsigned long hCount);
    bool timeup();
    bool timeup(unsigned long thisTarget);
  
};

#endif
