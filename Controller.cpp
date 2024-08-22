#include <Arduino.h>
#include <EEPROM.h>
#include "Controller.h"

#define MIN_POWER 0
#define MAX_POWER 255

Controller::Controller (float *sensor, uint8_t pinNumber)
  : pidControl(&sensorDouble, &power, &target, 2, 5, 1, DIRECT) {
  _sensor = sensor;
  sensorDouble = *_sensor;
  pin = pinNumber;
  power = (MAX_POWER - MIN_POWER) / 2;
  target = 0;

  pidControl.SetOutputLimits(MIN_POWER,MAX_POWER);
  pidControl.SetMode(AUTOMATIC);
  modified = false;
}

void Controller::setTarget (float targetValue) {
  target = targetValue;
  modified = true;
}

void Controller::setMaxError (float max) {
  maxError = max;
}

float Controller::getTarget (void) {
  return target;
}

uint8_t Controller::getPower (void) {
  return power;
}

float* Controller::getSensor(void) {
  return _sensor;
}

void Controller::control (void) {
  sensorDouble = *_sensor;
  pidControl.Compute();
  analogWrite (pin, power);
}


void Controller::save(int address) {
  unsigned int i;
  float targetFloat = target;
  byte *b = (byte *)&targetFloat;

  if (modified) {
    for (i=0; i<sizeof(targetFloat); i++) {
      EEPROM.write(address+i, *b++);
    }
  }
  
  modified = false;
}

void Controller::restore(int address) {
  unsigned int i;
  float targetFloat;
  byte *b = (byte *)&targetFloat;

  for (i=0; i<sizeof(targetFloat); i++) {
    *b++ = EEPROM.read(address+i);
  }

  target = targetFloat;
  
  modified = false;
}

bool Controller::getStatus() {
  return fabs(*_sensor - target) <= maxError;
}
