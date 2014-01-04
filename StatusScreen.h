#ifndef _STATUSSCREEN_
#define _STATUSSCREEN_

#include <LiquidCrystal.h>
#include "Observer.h"
#include "Subject.h"
#include "Sensor.h"
#include "TimerTrigger.h"
#include "Screen.h"

class StatusScreen: public Observer, public Screen
{
private:
  LiquidCrystal *_lcd;
  Sensor *_temperatureSensor;
  Sensor *_humiditySensor;
  TimerTrigger *_timerTrigger;
  
public:
  void init(LiquidCrystal *lcd, Sensor *temperatureSensor, 
	    Sensor *humiditySensor, TimerTrigger *timerTrigger);
  void update(Subject *subject);
  void activate(bool act);
  
private:
  void updateSensor(Sensor *sensor, uint8_t col, uint8_t row, char measureUnit);
};

#endif
