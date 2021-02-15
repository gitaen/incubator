#include "ControllerPIDScreen.h"

void ControllerPidScreen::init(LiquidCrystal *lcd, const __FlashStringHelper *nameString,
			    Controller *controller) {
  _lcd = lcd;
  _controller = controller;
  name = nameString;
}

void ControllerPidScreen::refresh () {
  _lcd->clear();
  _lcd->print(name);
  _lcd->print(F(" PID"));
  _lcd->print(':');
  _lcd->setCursor(0,1);
  if (_controller->isAutoTuningEnabled()){
    _lcd->print(F("AutoTuning..."));
  }
  else {
    _lcd->print(String(_controller->getKp(), 1));
    _lcd->print(String(_controller->getKi(), 1));
    _lcd->print(String(_controller->getKd(), 1));
  }
}

void ControllerPidScreen::modify(float step) {
  _controller->enableAutoTuning();
  refresh();
}


