#include <stdlib.h>
#include <string.h>
#include "StatusScreen.h"
#include "floatToString.h"
#include <HardwareSerial.h>

void StatusScreen::init(LiquidCrystal *lcd, Sensor *temperatureSensor, 
			   Sensor *humiditySensor, TimerTrigger *timerTrigger) {
  _temperatureSensor = temperatureSensor;
  _humiditySensor = humiditySensor;
  _timerTrigger = timerTrigger;

  activated = false;

  _temperatureSensor->attach(this);
  _humiditySensor->attach(this);
  _timerTrigger->attach(this);
}

void StatusScreen::update (Subject *subject) {
  char str[3];
  unsigned long timeLeft;
  
  if (activated) {
    if (subject == _temperatureSensor) {
      updateSensor (_temperatureSensor, 2, 0);
    }
    if (subject == _humiditySensor) {
      updateSensor (_humiditySensor, 10, 0);
    } else if (subject == _timerTrigger) {
      _lcd->setCursor(0,1);
      if (_timerTrigger->isActive()){
	if (_timerTrigger->getOnState()) {
	  _lcd->print("Eggs turning    ");
	} else {
	  _lcd->print("Next:           ");
	  _lcd->setCursor(8,1);
	  timeLeft = _timerTrigger->getTimeLeft();
	  itoa(timeLeft/3600, str, 10);
	  _lcd->print(str);
	  _lcd->print(':');
	  itoa((timeLeft%3600)/60, str, 10);
	  if (strlen(str) < 2)
	    _lcd->print("0");
	  _lcd->print(str);
	  _lcd->print(':');
	  itoa(timeLeft%60, str, 10);
	  if (strlen(str) < 2)
	    _lcd->print("0");
	  _lcd->print(str);
	}
      }
      else {
	_lcd->print("Turner off");
      }
    }
  }
}


void StatusScreen::activate(bool act) {
  
  Screen::activate(act);

  if (activated) {
    _lcd->clear();
    _lcd->print("T:");
    _lcd->setCursor(8,0);
    _lcd->print("H:");

    update(_temperatureSensor);
    update(_humiditySensor);
    update(_timerTrigger);
  }
}


void StatusScreen::updateSensor(Sensor *sensor, uint8_t col, uint8_t row) {
  float tempFloat;
  char str[6];
  char measureUnits;

  measureUnits = sensor->getMeasureUnits();
  tempFloat = sensor->getMeasurement();
  floatToString(str, tempFloat, 1, 4, true);
  _lcd->setCursor(col,row);
  _lcd->print(str);
  _lcd->print(measureUnits);
}

