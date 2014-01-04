#include "HumiditySensor.h"

HumiditySensor::HumiditySensor () {
  _humidity = 0;
}

void HumiditySensor::update (float newHumid) {
  if (newHumid!= _humidity) {
    _humidity = newHumid;
    notify();   
  }
}

float HumiditySensor::getMeasurement(void) {
  return _humidity;
}

char HumiditySensor::getMeasureUnits(void) {
  return '%';
}
