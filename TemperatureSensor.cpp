#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor () {
  _temperature = 0;
}

void TemperatureSensor::update (float newTemp) {
  if (newTemp != _temperature) {
    _temperature = newTemp;
    notify();   
  }
}

float TemperatureSensor::getMeasurement(void) {
  return _temperature;
}

char TemperatureSensor::getMeasureUnits(void) {
  return 'C';
}
