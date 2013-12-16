#ifndef _CONTROLLERSCREEN_
#define _CONTROLLERSCREEN_

#include <LiquidCrystal.h>
#include "Observer.h"
#include "Subject.h"
#include "Controller.h"
#include "Screen.h"

#define MINVALUE 0.0
#define MAXVALUE 100.0

class ControllerScreen: public Observer, public Screen
{
private:
  LiquidCrystal *_lcd;
  Controller *_controller;
  char *name;
  
public:
  void init(LiquidCrystal *lcd, char *nameString, Controller *controller);
  void modify(float step);
  void update(Subject *subject);
  void activate(bool act);

};

#endif
