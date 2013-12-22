#include <Arduino.h>
#include <EEPROM.h>
#include "Controller.h"

#define MIN_POWER 0
#define MAX_POWER 255

void Controller::init (Sensor *sensor, uint8_t pinNumber,
		       float errorPoints[NUMBEROFFUZZYSETS],
		       uint8_t errorFunctions[NUMBEROFFUZZYSETS][NUMBEROFFUZZYSETS],
		       float deltaPoints[NUMBEROFFUZZYSETS],
		       uint8_t deltaFunctions[NUMBEROFFUZZYSETS][NUMBEROFFUZZYSETS],
		       uint8_t rules[NUMBEROFFUZZYSETS][NUMBEROFFUZZYSETS],
		       float outputFunction[NUMBEROFFUZZYSETS]){
  _sensor = sensor;
  pin = pinNumber;
  power = (MAX_POWER - MIN_POWER) / 2;
  target = 0;

  lastError = _sensor->getMeasurement() - target;
  lastTime = millis();
  
  _errorPoints = errorPoints;
  _errorFunctions = errorFunctions;
  _deltaPoints = deltaPoints,
  _deltaFunctions = deltaFunctions;
  _rules = rules;
  _outputFunction = outputFunction;
  modified = false;

}

void Controller::setTarget (float targetValue) {
  target = targetValue;
  modified = true;
  notify();
}

float Controller::getTarget (void) {
  return target;
}

uint8_t Controller::getPower (void) {
  return power;
}

Sensor* Controller::getSensor(void) {
  return _sensor;
}

void Controller::control (void) {
  float error;
  float delta;
  float throttle;
  uint8_t i;
  float elapsedSeconds;

  float errorArray[NUMBEROFFUZZYSETS];
  float deltaArray[NUMBEROFFUZZYSETS];
//  float rulesResultArray[NUMBEROFFUZZYSETS][NUMBEROFFUZZYSETS];

  error = _sensor->getMeasurement() - target;
  elapsedSeconds = (float)(millis() - lastTime) /1000;

  delta = (error - lastError)/elapsedSeconds; // TODO: check roll over

  lastTime = millis();
  lastError = error;

  for (i=0; i<NUMBEROFFUZZYSETS; i++) {
    errorArray[i] = fuzzify(error, _errorPoints, _errorFunctions[i]);
    deltaArray[i] = fuzzify(delta, _deltaPoints, _deltaFunctions[i]);
  }
//  infere(errorArray, deltaArray, rulesResultArray);
  throttle = defuzzify(errorArray, deltaArray);

  // throttle = throttle * elapsedSeconds;

  if (throttle > 0)
    throttle += 0.5; //to round it
  else if (throttle < 0)
    throttle -= 0.5;

  power = (uint8_t)constrain(power + 
			     throttle, MIN_POWER, MAX_POWER);

  analogWrite (pin, power);
  
  if ((int)throttle)
    notify();
}


void Controller::save(int address) {
  int i;
  byte *b = (byte *)&target;

  if (modified) {
    for (i=0; i<sizeof(float); i++) {
      EEPROM.write(address+i, *b++);
    }
  }
  
  modified = false;
}

void Controller::restore(int address) {
  int i;
  byte *b = (byte *)&target;

  for (i=0; i<sizeof(float); i++) {
    *b++ = EEPROM.read(address+i);
  }
  
  modified = false;
  notify();
}


float Controller::fuzzify(float error, float points[], uint8_t function[]) {
  float result;
  int8_t i;
  uint8_t bottom=0;
  uint8_t top=NUMBEROFFUZZYSETS-1;

  for (i=0; i<NUMBEROFFUZZYSETS; i++) {
    if (points[i] <= error)
      bottom = i;
  }
  
  for (i=NUMBEROFFUZZYSETS-1; i>= 0; i--) {
    if (points[i] >= error)
      top = i;
  }

  if (bottom == top)
    result = function[bottom];
  else
    result = function[bottom] + (error - points[bottom]) * (function[top] - function[bottom])/(points[top] - points[bottom]);

  return result;
}


//void Controller::infere(float *errorArray, float *deltaArray, 
//			     float rulesResultArray[NUMBEROFFUZZYSETS][NUMBEROFFUZZYSETS]) {
//  uint8_t i;
//  uint8_t j;
//
//  for (i=0; i < NUMBEROFFUZZYSETS; i++){
//    for (j=0; j < NUMBEROFFUZZYSETS; j ++) {
//      rulesResultArray[i][j] = min (errorArray[i], deltaArray[j]);
//    }
//  }
//}

float Controller::defuzzify(float *errorArray, float *deltaArray) {

  float outputSum = 0;
  float rulesSum = 0;
  uint8_t i;
  uint8_t j;

  for (i=0; i < NUMBEROFFUZZYSETS; i++){
    for (j=0; j < NUMBEROFFUZZYSETS; j ++) {
      outputSum += min (errorArray[i], deltaArray[j]) * _outputFunction[_rules[i][j]];
      rulesSum += min (errorArray[i], deltaArray[j]);
    }
  }

  return outputSum/rulesSum;
}
