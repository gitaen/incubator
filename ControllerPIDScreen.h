#ifndef _CONTROLLERPIDSCREEN_
#define _CONTROLLERPIDSCREEN_

#include <LiquidCrystal.h>
#include "Controller.h"
#include "Screen.h"

class ControllerPidScreen: public Screen
{
private:
  LiquidCrystal *_lcd;
  Controller *_controller;
  const __FlashStringHelper *name;
  
public:
  void init(LiquidCrystal *lcd, const __FlashStringHelper *nameString, Controller *controller);
  void modify(float step);
  void refresh();

};

#endif
