#include <Arduino.h>
#include <EEPROM.h>
#include "Controller.h"
#include <MemoryFree.h>

#define MIN_POWER 0
#define MAX_POWER 255

#define NUM_RULES 7

//#define DEBUG

Fuzzy *Controller::fuzzy = NULL;

FuzzyInput Controller::errorInput = FuzzyInput(1);
FuzzySet Controller::L_N = FuzzySet(-2, -2, -1, -0.25);
FuzzySet Controller::S_N = FuzzySet(-1, -0.25, -0.1, 0);
FuzzySet Controller::Z = FuzzySet(-0.1, 0, 0, 0.1);
FuzzySet Controller::S_P = FuzzySet(0, 0.1, 0.25, 1);
FuzzySet Controller::L_P = FuzzySet(0.25, 1, 2, 2);

FuzzyInput Controller::errorDeltaInput = FuzzyInput(2);
FuzzySet Controller::L_NA = FuzzySet(-0.02, -0.02, -0.01, -0.005);
FuzzySet Controller::S_NA = FuzzySet(-0.01, -0.005, -0.005, 0);
FuzzySet Controller::Z_A = FuzzySet(-0.005, 0, 0, 0.005);
FuzzySet Controller::S_PA = FuzzySet(0, 0.005, 0.005, 0.01);
FuzzySet Controller::L_PA = FuzzySet(0.005, 0.01, 0.02, 0.02);

FuzzyOutput Controller::adjust = FuzzyOutput(1);
FuzzySet Controller::L_D = FuzzySet(-64, -64, -64, 0);
FuzzySet Controller::S_D = FuzzySet(-20, -20, -20, 0);
FuzzySet Controller::K = FuzzySet(0,0,0,0);
FuzzySet Controller::S_I = FuzzySet(0, 20, 20, 20);
FuzzySet Controller::L_I = FuzzySet(0, 64, 64, 64);

extern unsigned int debugTmp;

Controller::Controller (float *sensor, uint8_t pinNumber) {
  int i = 1;
  _sensor = sensor;
  pin = pinNumber;
  power = (MAX_POWER - MIN_POWER) / 2;
  target = 0;

  lastError = *_sensor - target;
  lastTime = millis();
  
  modified = false;
  // Serial.begin(9600);
  if (fuzzy == NULL) {
    fuzzy = new Fuzzy();

    // Serial.println((unsigned long int)errorInput.fuzzySets);
    // Serial.println((unsigned long int)errorInput.fuzzySetsCursor);
    errorInput.addFuzzySet(&L_N);
    // Serial.println((unsigned long int)errorInput.fuzzySets);
    // Serial.println((unsigned long int)errorInput.fuzzySetsCursor);
    errorInput.addFuzzySet(&S_N);
    // Serial.println((unsigned long int)errorInput.fuzzySets);
    // Serial.println((unsigned long int)errorInput.fuzzySetsCursor);
    errorInput.addFuzzySet(&Z);
    // Serial.println((unsigned long int)errorInput.fuzzySets);
    // Serial.println((unsigned long int)errorInput.fuzzySetsCursor);
    errorInput.addFuzzySet(&S_P);
    // Serial.println((unsigned long int)errorInput.fuzzySets);
    // Serial.println((unsigned long int)errorInput.fuzzySetsCursor);
    errorInput.addFuzzySet(&L_P);
    // Serial.println((unsigned long int)errorInput.fuzzySets);
    // Serial.println((unsigned long int)errorInput.fuzzySetsCursor);
    fuzzy->addFuzzyInput(&errorInput);

    errorDeltaInput.addFuzzySet(&L_NA);
    errorDeltaInput.addFuzzySet(&S_NA);
    errorDeltaInput.addFuzzySet(&Z_A);
    errorDeltaInput.addFuzzySet(&S_PA);
    errorDeltaInput.addFuzzySet(&L_PA);
    fuzzy->addFuzzyInput(&errorDeltaInput);

    adjust.addFuzzySet(&L_D);
    adjust.addFuzzySet(&S_D);
    adjust.addFuzzySet(&K);
    adjust.addFuzzySet(&S_I);
    adjust.addFuzzySet(&L_I);
    fuzzy->addFuzzyOutput(&adjust);

    FuzzyRuleAntecedent* ifLargeNegative = new FuzzyRuleAntecedent();
    ifLargeNegative->joinSingle(&L_N);
    FuzzyRuleAntecedent* Decreasing = new FuzzyRuleAntecedent();
    Decreasing->joinWithOR(&L_NA, &S_NA);
    FuzzyRuleAntecedent* NotIncreasing = new FuzzyRuleAntecedent();
    NotIncreasing->joinWithOR(Decreasing, &Z_A);
    FuzzyRuleAntecedent* ifSmallNegativeAndNotIncreasing = new FuzzyRuleAntecedent();
    ifSmallNegativeAndNotIncreasing->joinWithAND(&S_N,NotIncreasing);
    FuzzyRuleAntecedent* ifSmallNegativeAndQuicklyIncreasing = new FuzzyRuleAntecedent();
    ifSmallNegativeAndQuicklyIncreasing->joinWithAND(&S_N, &L_PA);
    FuzzyRuleAntecedent* ifZero = new FuzzyRuleAntecedent();
    ifZero->joinSingle(&Z);
    FuzzyRuleAntecedent* Increasing = new FuzzyRuleAntecedent();
    Increasing->joinWithOR(&L_PA, &S_PA);
    FuzzyRuleAntecedent* NotDecreasing = new FuzzyRuleAntecedent();
    NotDecreasing->joinWithOR(Increasing, &Z_A);
    FuzzyRuleAntecedent* ifSmallPositiveAndNotDecreasing = new FuzzyRuleAntecedent();
    ifSmallPositiveAndNotDecreasing->joinWithAND(&S_P,NotDecreasing);
    FuzzyRuleAntecedent* ifSmallPositiveAndQuicklyDecreasing = new FuzzyRuleAntecedent();
    ifSmallPositiveAndQuicklyDecreasing->joinWithAND(&S_P, &L_NA);
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
    
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifLargeNegative, LargeIncrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifSmallNegativeAndNotIncreasing, SmallIncrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifSmallNegativeAndQuicklyIncreasing, SmallDecrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifZero, Keep));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifSmallPositiveAndQuicklyDecreasing, SmallIncrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifSmallPositiveAndNotDecreasing, SmallDecrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifLargePositive, LargeDecrease));
  } 
}

void Controller::setTarget (float targetValue) {
  target = targetValue;
  modified = true;
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
  float error;
  float delta;
  float throttle;
  uint8_t i;
  float elapsedSeconds;

  error = *_sensor - target;
  elapsedSeconds = (float)(millis() - lastTime) /1000;

  delta = (error - lastError)/elapsedSeconds; // TODO: check roll over

  lastTime = millis();
  lastError = error;

  fuzzy->setInput(1, error);
  fuzzy->setInput(2, delta);
  fuzzy->fuzzify();

#ifdef DEBUG
  Serial.println(error,3);
  Serial.println(delta,3);

  Serial.print(L_N.getPertinence());
  Serial.print(' ');
  Serial.print(S_N.getPertinence());
  Serial.print(' ');
  Serial.print(Z.getPertinence());
  Serial.print(' ');
  Serial.print(S_P.getPertinence());
  Serial.print(' ');
  Serial.println(L_P.getPertinence());

  Serial.print(L_NA.getPertinence());
  Serial.print(' ');
  Serial.print(S_NA.getPertinence());
  Serial.print(' ');
  Serial.print(Z_A.getPertinence());
  Serial.print(' ');
  Serial.print(S_PA.getPertinence());
  Serial.print(' ');
  Serial.println(L_PA.getPertinence());

  for (i=1; i<=NUM_RULES; i++) {
    Serial.print(fuzzy->isFiredRule(i));
    Serial.print(' ');
  }
  Serial.println();

  Serial.print(L_D.getPertinence());
  Serial.print(' ');
  Serial.print(S_D.getPertinence());
  Serial.print(' ');
  Serial.print(K.getPertinence());
  Serial.print(' ');
  Serial.print(S_I.getPertinence());
  Serial.print(' ');
  Serial.println(L_I.getPertinence());
#endif

  throttle = fuzzy->defuzzify(1);

#ifdef DEBUG
  Serial.println(throttle);
#endif

  if (throttle > 0)
    throttle += 0.5; //to round it
  else if (throttle < 0)
    throttle -= 0.5;

  power = (uint8_t)constrain(power + 
   			     throttle, MIN_POWER, MAX_POWER);

  analogWrite (pin, power);
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
}
