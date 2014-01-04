#ifndef _SERIALCOMM_
#define _SERIALCOMM_

#include "Sensor.h"
#include "TimerTrigger.h"
#include "Observer.h"
#include "Controller.h"

class SerialComm
{
private:
  TimerTrigger *_eggTurner;
  Controller *_tempController;
  Controller *_humidController;

public:
  void init(TimerTrigger *eggTurner, Controller *tempController, Controller *humidController);
  void refresh();
};

#endif
