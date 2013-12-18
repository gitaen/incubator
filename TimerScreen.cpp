#include <stdlib.h>
#include "TimerScreen.h"
#include "MemoryTest.h"

void TimerScreen::init(LiquidCrystal *lcd, char *nameString, 
			    TimerTrigger *timer) {
  _lcd = lcd;
  _timer = timer;
  name = nameString;

  activated = false;

  _timer->attach(this);
}

void TimerScreen::update (Subject *subject) {
  if (activated) {
    _lcd->setCursor(0,1);
    if (_timer->isActive())
      _lcd->print("on ");
    else
      _lcd->print("off");
  }
}


void TimerScreen::activate(bool act) {
  
  Screen::activate(act);

  if (activated) {
    _lcd->clear();
    _lcd->print("Set ");
    _lcd->print(name);
    _lcd->print(":");
    update(_timer);
   }
}


void TimerScreen::modify(float step) {
  float target;

  _timer->activate(!_timer->isActive());
}


