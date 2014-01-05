#ifndef _CONTROLLERSCREEN_
#define _CONTROLLERSCREEN_

#include <LiquidCrystal.h>
#include "Controller.h"
#include "Screen.h"

#define MINVALUE 0.0
#define MAXVALUE 100.0

class ControllerScreen: public Screen
{
private:
  LiquidCrystal *_lcd;
  Controller *_controller;
  char _measureUnit;
  const __FlashStringHelper *name;
  
public:
  void init(LiquidCrystal *lcd, const __FlashStringHelper *nameString, Controller *controller,
	    char measureUnit);
  void modify(float step);
  void refresh();

};

#endif
