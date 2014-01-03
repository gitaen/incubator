#ifndef _SERIALCOMM_
#define _SERIALCOMM_

#include "Sensor.h"
#include "TimerTrigger.h"
#include "Observer.h"
#include "Controller.h"

class SerialComm: public Observer
{
private:
  Sensor *_temperatureSensor;
  Sensor *_humiditySensor;
  TimerTrigger *_eggTurner;
  Controller *_tempController;
  Controller *_humidController;
  float temp;
  float humid;
  float tempTarget;
  float humidTarget;
  uint8_t tempPower;
  uint8_t humidPower;
  bool turnerActive;
  bool turnerState;
  unsigned long int turnerRemTime;

public:
  void init(Sensor *temp, Sensor *humidity, TimerTrigger *eggTurner, 
	    Controller *tempController, Controller *humidController);
  void update(Subject* subject);
  void refresh();
};

#endif
