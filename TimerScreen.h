#ifndef _TIMERSCREEN_
#define _TIMERSCREEN_

#include <LiquidCrystal.h>
#include "Observer.h"
#include "Subject.h"
#include "TimerTrigger.h"
#include "Screen.h"

class TimerScreen: public Observer, public Screen
{
private:
  LiquidCrystal *_lcd;
  TimerTrigger *_timer;
  const __FlashStringHelper *name;
  
public:
  void init(LiquidCrystal *lcd, const __FlashStringHelper *nameString, TimerTrigger *timer);
  void modify(float step);
  void update(Subject *subject);
  void activate(bool act);

};

#endif
