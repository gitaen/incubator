#include "ControllerScreen.h"
#include "MemoryTest.h"

void ControllerScreen::init(LiquidCrystal *lcd, const __FlashStringHelper *nameString, 
			    Controller *controller, char measureUnit) {
  _lcd = lcd;
  _controller = controller;
  _measureUnit = measureUnit;
  name = nameString;

  activated = false;

  _controller->attach(this);
}

void ControllerScreen::update (Subject *subject) {
  char str[5];
  
  if (activated) {
    _lcd->setCursor(0,1);
    dtostrf(_controller->getTarget(), -4, 1, str);
    _lcd->print(str);
    _lcd->print(_measureUnit);
  }
}


void ControllerScreen::activate(bool act) {
  
  Screen::activate(act);

  if (activated) {
    _lcd->clear();
    _lcd->print(F("Set "));
    _lcd->print(name);
    _lcd->print(':');
    update(_controller);
   }
}


void ControllerScreen::modify(float step) {
  float target;

  target = _controller->getTarget();

  target += step;

  if (target > MAXVALUE)
    target = MINVALUE;
  if (target < MINVALUE)
    target = MAXVALUE;

  _controller->setTarget(target);
}


