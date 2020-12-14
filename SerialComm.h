#ifndef _SERIALCOMM_
#define _SERIALCOMM_

#include "TimerTrigger.h"
#include "Controller.h"

class SerialComm
{
 private:
  TimerTrigger *_eggTurner;
  Controller *_tempController;
  Controller *_humidController;
  
 public:
  void init(TimerTrigger *eggTurner, Controller *tempController, Controller *humidController);
  void check();
 private:
  void getParameters();
  void setParameters();
};

#endif
