#include <Arduino.h>
#include <EEPROM.h>
#include "Controller.h"

#define MIN_POWER 0
#define MAX_POWER 255

struct config {
  float target;
  double kP;
  double kI;
  double kD;
};

Controller::Controller (float *sensor, uint8_t pinNumber, unsigned int sampleTime)
  : pidControl(&sensorDouble, &power, &target, 2, 5, 1, DIRECT),
    autoTune(&sensorDouble, &power)
{
  _sensor = sensor;
  sensorDouble = *_sensor;
  pin = pinNumber;
  power = (MAX_POWER - MIN_POWER) / 2;
  target = 0;
  autoTuneEnabled = false;

  pidControl.SetOutputLimits(MIN_POWER,MAX_POWER);
  pidControl.SetMode(AUTOMATIC);
  pidControl.SetSampleTime(sampleTime);

  autoTune.SetControlType(1);
  autoTune.SetNoiseBand(0.1);
  autoTune.SetLookbackSec(30);
  modified = false;
}

void Controller::enableAutoTuning(void)
{
    autoTuneEnabled = true;
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

void Controller::runAutoTune(void) {
  if (autoTune.Runtime()) {
    pidControl.SetTunings(autoTune.GetKp(), autoTune.GetKi(), autoTune.GetKd());
    autoTuneEnabled = false;
    modified = true;
  }
}

void Controller::runController(void) {
  sensorDouble = *_sensor;
  pidControl.Compute();
  analogWrite (pin, power);
}

void Controller::control (void) {
  if (autoTuneEnabled) {
    runAutoTune();
  }
  else {
    runController();
  }
}


void Controller::save(int address) {
  struct config config = {
    .target = target,
    .kP = pidControl.GetKp(),
    .kI = pidControl.GetKi(),
    .kD = pidControl.GetKd(),
  };

  if (modified) {
    EEPROM.put(address, config);
  }
  
  modified = false;
}

void Controller::restore(int address) {
  struct config config;

  config = EEPROM.get(address, config);

  target = config.target;
  pidControl.SetTunings(config.kP, config.kI, config.kD);
  
  modified = false;
}

bool Controller::getStatus() {
  return fabs(*_sensor - target) <= maxError;
}

bool Controller::isAutoTuningEnabled(void) {
  return autoTuneEnabled;
}

size_t Controller::getConfigSize(void)
{
  return sizeof(struct config);
}
