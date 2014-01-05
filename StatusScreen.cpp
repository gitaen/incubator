#include <stdlib.h>
#include <string.h>
#include "StatusScreen.h"
#include <HardwareSerial.h>

void StatusScreen::init(LiquidCrystal *lcd, float *temperature, 
			   float *humidity, TimerTrigger *timerTrigger) {
  _lcd = lcd;
  _temperature = temperature;
  _humidity = humidity;
  _timerTrigger = timerTrigger;
}

void StatusScreen::refresh () {
  char str[3];
  unsigned long timeLeft;
  
  _lcd->clear();
  _lcd->print(F("T:"));
  _lcd->setCursor(8,0);
  _lcd->print(F("H:"));
  
  updateSensor (_temperature, 2, 0, 'C');
  updateSensor (_humidity, 10, 0, '%');
  _lcd->setCursor(0,1);
  if (_timerTrigger->isActive()){
    if (_timerTrigger->getOnState()) {
      _lcd->print(F("Eggs turning    "));
    } else {
      _lcd->print(F("Turn in         "));
      _lcd->setCursor(8,1);
      timeLeft = _timerTrigger->getTimeLeft();
      itoa(timeLeft/3600, str, 10);
      _lcd->print(str);
      _lcd->print(':');
      itoa((timeLeft%3600)/60, str, 10);
      if (strlen(str) < 2)
	_lcd->print('0');
      _lcd->print(str);
      _lcd->print(':');
      itoa(timeLeft%60, str, 10);
      if (strlen(str) < 2)
	_lcd->print('0');
      _lcd->print(str);
    }
  }
  else {
    _lcd->print(F("Turner off"));
  }
}


void StatusScreen::updateSensor(const float *sensor, uint8_t col, uint8_t row, char measureUnit)
{
  char str[5];

  dtostrf(*sensor, 4, 1, str);
  _lcd->setCursor(col,row);
  _lcd->print(str);
  _lcd->print(measureUnit);
}

