#include "Sensor.h"

void Sensor::update (float newValue) {
  if (newValue!= value) {
    value = newValue;
    notify();   
  }
}

float Sensor::getMeasurement(void) {
  return value;
}
