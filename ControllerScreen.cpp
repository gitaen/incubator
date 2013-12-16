//#include <stdlib.h>
#include "ControllerScreen.h"
#include "floatToString.h"
#include "MemoryTest.h"

void ControllerScreen::init(LiquidCrystal *lcd, char *nameString, 
			    Controller *controller) {
  _lcd = lcd;
  _controller = controller;
  name = nameString;

  activated = false;

  _controller->attach(this);
}

void ControllerScreen::update (Subject *subject) {
  char str[7];
  
//   int result = memoryTest();
//   Serial.print("Update: ");
//   Serial.println(result,DEC);
  
//  Serial.println("Update");
  if (activated) {
    _lcd->setCursor(0,1);
    floatToString(str, _controller->getTarget(), 1, 5, true);
    _lcd->print(str);
    _lcd->print(_controller->getSensor()->getMeasureUnits());
  }
}


void ControllerScreen::activate(bool act) {
  
  Screen::activate(act);

  if (activated) {
    _lcd->clear();
    _lcd->print("Set ");
    _lcd->print(name);
    _lcd->print(":");
    update(_controller);
   }
}


void ControllerScreen::modify(float step) {
  float target;

//   int result = memoryTest();
//   Serial.print("Modify: ");
//   Serial.println(result,DEC);

  target = _controller->getTarget();

  target += step;

  if (target > MAXVALUE)
    target = MINVALUE;
  if (target < MINVALUE)
    target = MAXVALUE;

  //  Serial.println("Modify");
  _controller->setTarget(target);
}


