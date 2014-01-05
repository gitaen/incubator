#ifndef _TIMERSCREEN_
#define _TIMERSCREEN_

#include <LiquidCrystal.h>
#include "TimerTrigger.h"
#include "Screen.h"

class TimerScreen: public Screen
{
private:
  LiquidCrystal *_lcd;
  TimerTrigger *_timer;
  const __FlashStringHelper *name;
  
public:
  void init(LiquidCrystal *lcd, const __FlashStringHelper *nameString, TimerTrigger *timer);
  void modify(float step);
  void refresh();

};

#endif
