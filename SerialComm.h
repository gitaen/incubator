#ifndef _SERIALCOMM_
#define _SERIALCOMM_

#include "Sensor.h"
#include "TimerTrigger.h"
#include "Observer.h"

class SerialComm: public Observer
{
private:
  Sensor *_temperatureSensor;
  Sensor *_humiditySensor;
  TimerTrigger *_eggTurner;

public:
  void init(Sensor *temp, Sensor *humidity, TimerTrigger *eggTurner);
  void update(Subject* subject);
};

#endif
