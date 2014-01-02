#include <Arduino.h>
#include <EEPROM.h>
#include "Controller.h"

#define MIN_POWER 0
#define MAX_POWER 255

#define NUM_RULES 5

Fuzzy *Controller::fuzzy = NULL;

FuzzyInput Controller::error = FuzzyInput(1);
FuzzySet Controller::L_N = FuzzySet(-1, -1, -1, -0.5);
FuzzySet Controller::S_N = FuzzySet(-1, -0.5, -0.5, 0);
FuzzySet Controller::Z = FuzzySet(-0.5, 0, 0, 0-5);
FuzzySet Controller::S_P = FuzzySet(0, 0.5, 0.5, 1);
FuzzySet Controller::L_P = FuzzySet(0.5, 1, 1 ,1 );

FuzzyOutput Controller::adjust = FuzzyOutput(1);
FuzzySet Controller::L_D = FuzzySet(-8, -8, -8, -2);
FuzzySet Controller::S_D = FuzzySet(-4, -2, -2, 0);
FuzzySet Controller::K = FuzzySet(0,0,0,0);
FuzzySet Controller::S_I = FuzzySet(0, 2, 2, 4);
FuzzySet Controller::L_I = FuzzySet(2, 8, 8, 8);


Controller::Controller (Sensor *sensor, uint8_t pinNumber) {
  FuzzyRule *rules[NUM_RULES];
  int i = 0;
  _sensor = sensor;
  pin = pinNumber;
  power = (MAX_POWER - MIN_POWER) / 2;
  target = 0;

  lastError = _sensor->getMeasurement() - target;
  lastTime = millis();
  
  modified = false;
  
  if (fuzzy == NULL) {
    fuzzy = new Fuzzy();

    error.addFuzzySet(&L_N);
    error.addFuzzySet(&S_N);
    error.addFuzzySet(&Z);
    error.addFuzzySet(&S_P);
    error.addFuzzySet(&L_P);
    fuzzy->addFuzzyInput(&error);

    adjust.addFuzzySet(&L_D);
    adjust.addFuzzySet(&S_D);
    adjust.addFuzzySet(&K);
    adjust.addFuzzySet(&S_I);
    adjust.addFuzzySet(&L_I);
    fuzzy->addFuzzyOutput(&adjust);

    FuzzyRuleAntecedent* ifLargeNegative = new FuzzyRuleAntecedent();
    ifLargeNegative->joinSingle(&L_N);
    FuzzyRuleAntecedent* ifSmallNegative = new FuzzyRuleAntecedent();
    ifSmallNegative->joinSingle(&S_N);
    FuzzyRuleAntecedent* ifZero = new FuzzyRuleAntecedent();
    ifZero->joinSingle(&Z);
    FuzzyRuleAntecedent* ifSmallPositive = new FuzzyRuleAntecedent();
    ifSmallPositive->joinSingle(&S_P);
    FuzzyRuleAntecedent* ifLargePositive = new FuzzyRuleAntecedent();
    ifLargePositive->joinSingle(&L_P);

    FuzzyRuleConsequent* LargeDecrease = new FuzzyRuleConsequent();
    LargeDecrease->addOutput(&L_D);
    FuzzyRuleConsequent* SmallDecrease = new FuzzyRuleConsequent();
    SmallDecrease->addOutput(&S_D);
    FuzzyRuleConsequent* Keep = new FuzzyRuleConsequent();
    Keep->addOutput(&K);
    FuzzyRuleConsequent* SmallIncrease = new FuzzyRuleConsequent();
    SmallIncrease->addOutput(&S_I);
    FuzzyRuleConsequent* LargeIncrease = new FuzzyRuleConsequent();
    LargeIncrease->addOutput(&L_I);
    
    rules[i++] = new FuzzyRule(i, ifLargeNegative, LargeIncrease);
    rules[i++] = new FuzzyRule(i, ifSmallNegative, SmallIncrease);
    rules[i++] = new FuzzyRule(i, ifZero, Keep);
    rules[i++] = new FuzzyRule(i, ifSmallPositive, SmallDecrease);
    rules[i++] = new FuzzyRule(i, ifLargePositive, LargeDecrease);

    for(i=0; i<NUM_RULES; i++) {
      fuzzy->addFuzzyRule(rules[i]);
    }
  } 
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

  error = _sensor->getMeasurement() - target;
  elapsedSeconds = (float)(millis() - lastTime) /1000;

  delta = (error - lastError)/elapsedSeconds; // TODO: check roll over

  lastTime = millis();
  lastError = error;

  fuzzy->setInput(1, error);
  fuzzy->fuzzify();
  throttle = fuzzy->defuzzify(1);

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
