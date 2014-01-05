#ifndef _STATUSSCREEN_
#define _STATUSSCREEN_

#include <LiquidCrystal.h>
#include "TimerTrigger.h"
#include "Screen.h"

class StatusScreen: public Screen
{
private:
  LiquidCrystal *_lcd;
  float *_temperature;
  float *_humidity;
  TimerTrigger *_timerTrigger;
  
public:
  void init(LiquidCrystal *lcd, float *temperature, 
	    float *humidity, TimerTrigger *timerTrigger);
  void refresh();
  
private:
  void updateSensor(const float *sensor, uint8_t col, uint8_t row, char measureUnit);
};

#endif
