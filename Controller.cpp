#include <Arduino.h>
#include <EEPROM.h>
#include "Controller.h"
#include <MemoryFree.h>

#define MIN_POWER 0
#define MAX_POWER 255

#define NUM_RULES 25

//#define DEBUG

Fuzzy *Controller::fuzzy = NULL;

FuzzyInput Controller::errorInput = FuzzyInput(1);
FuzzySet Controller::L_N = FuzzySet(-3, -3, -2.75, -0.2);
FuzzySet Controller::S_N = FuzzySet(-2.75, -0.2, -0.05, 0);
FuzzySet Controller::Z = FuzzySet(-0.05, 0, 0, 0.5);
FuzzySet Controller::S_P = FuzzySet(0, 0.5, 0.2, 2.75);
FuzzySet Controller::L_P = FuzzySet(0.2, 2.75, 3, 3);

FuzzyInput Controller::errorDeltaInput = FuzzyInput(2);
FuzzySet Controller::L_NA = FuzzySet(-0.1, -0.1, -0.02, -0.005);
FuzzySet Controller::S_NA = FuzzySet(-0.02, -0.005, -0.0025, 0);
FuzzySet Controller::Z_A = FuzzySet(-0.0025, 0, 0, 0.0025);
FuzzySet Controller::S_PA = FuzzySet(0, 0.0025, 0.005, 0.02);
FuzzySet Controller::L_PA = FuzzySet(0.005, 0.02, 0.1, 0.1);

FuzzyOutput Controller::adjust = FuzzyOutput(1);
FuzzySet Controller::VL_D = FuzzySet(-18, -16, -16, -14);
FuzzySet Controller::L_D = FuzzySet(-10, -8, -8, -6);
FuzzySet Controller::M_D = FuzzySet(-6, -4, -4, -2);
FuzzySet Controller::S_D = FuzzySet(-4, -2, -2, 0);
FuzzySet Controller::K = FuzzySet(-1,0,0,1);
FuzzySet Controller::S_I = FuzzySet(0, 2, 2, 4);
FuzzySet Controller::M_I = FuzzySet(2, 4, 4, 6);
FuzzySet Controller::L_I = FuzzySet(6, 8, 8, 10);
FuzzySet Controller::VL_I = FuzzySet(14, 16, 16, 18);

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

    adjust.addFuzzySet(&VL_D);
    adjust.addFuzzySet(&L_D);
    adjust.addFuzzySet(&M_D);
    adjust.addFuzzySet(&S_D);
    adjust.addFuzzySet(&K);
    adjust.addFuzzySet(&S_I);
    adjust.addFuzzySet(&M_I);
    adjust.addFuzzySet(&L_I);
    adjust.addFuzzySet(&VL_I);
    fuzzy->addFuzzyOutput(&adjust);

    FuzzyRuleAntecedent* ifLargeNegativeAndQuicklyDecreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifLargeNegativeAndSlowlyDecreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifLargeNegativeAndNotMoving = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifLargeNegativeAndSlowlyIncreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifLargeNegativeAndQuicklyIncreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifSmallNegativeAndQuicklyDecreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifSmallNegativeAndSlowlyDecreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifSmallNegativeAndNotMoving = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifSmallNegativeAndSlowlyIncreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifSmallNegativeAndQuicklyIncreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifZeroAndQuicklyDecreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifZeroAndSlowlyDecreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifZeroAndNotMoving = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifZeroAndSlowlyIncreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifZeroAndQuicklyIncreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifSmallPositiveAndSlowlyIncreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifSmallPositiveAndQuicklyIncreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifSmallPositiveAndNotMoving = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifSmallPositiveAndSlowlyDecreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifSmallPositiveAndQuicklyDecreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifLargePositiveAndQuicklyDecreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifLargePositiveAndSlowlyDecreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifLargePositiveAndNotMoving = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifLargePositiveAndSlowlyIncreasing = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifLargePositiveAndQuicklyIncreasing = new FuzzyRuleAntecedent();

    ifLargeNegativeAndQuicklyDecreasing->joinWithAND(&L_N, &L_NA);
    ifLargeNegativeAndSlowlyDecreasing->joinWithAND(&L_N, &S_NA);
    ifLargeNegativeAndNotMoving->joinWithAND(&L_N, &Z_A);
    ifLargeNegativeAndSlowlyIncreasing->joinWithAND(&L_N, &S_PA);
    ifLargeNegativeAndQuicklyIncreasing->joinWithAND(&L_N, &L_PA);
    ifSmallNegativeAndQuicklyDecreasing->joinWithAND(&S_N,&L_NA);
    ifSmallNegativeAndSlowlyDecreasing->joinWithAND(&S_N,&S_NA);
    ifSmallNegativeAndNotMoving->joinWithAND(&S_N,&Z_A);
    ifSmallNegativeAndSlowlyIncreasing->joinWithAND(&S_N, &S_PA);
    ifSmallNegativeAndQuicklyIncreasing->joinWithAND(&S_N, &L_PA);
    ifZeroAndQuicklyDecreasing->joinWithAND(&Z, &L_NA);
    ifZeroAndSlowlyDecreasing->joinWithAND(&Z, &S_NA);
    ifZeroAndNotMoving->joinWithAND(&Z, &Z_A);
    ifZeroAndSlowlyIncreasing->joinWithAND(&Z, &S_PA);
    ifZeroAndQuicklyIncreasing->joinWithAND(&Z, &L_PA);
    ifSmallPositiveAndSlowlyIncreasing->joinWithAND(&S_P,&S_PA);
    ifSmallPositiveAndQuicklyIncreasing->joinWithAND(&S_P,&L_PA);
    ifSmallPositiveAndNotMoving->joinWithAND(&S_P,&Z_A);
    ifSmallPositiveAndSlowlyDecreasing->joinWithAND(&S_P, &S_NA);
    ifSmallPositiveAndQuicklyDecreasing->joinWithAND(&S_P, &L_NA);
    ifLargePositiveAndQuicklyDecreasing->joinWithAND(&L_P, &L_NA);
    ifLargePositiveAndSlowlyDecreasing->joinWithAND(&L_P, &S_NA);
    ifLargePositiveAndNotMoving->joinWithAND(&L_P, &Z_A);
    ifLargePositiveAndSlowlyIncreasing->joinWithAND(&L_P, &S_PA);
    ifLargePositiveAndQuicklyIncreasing->joinWithAND(&L_P, &L_PA);

    FuzzyRuleConsequent* VeryLargeDecrease = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* LargeDecrease = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* MediumDecrease = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* SmallDecrease = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* Keep = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* SmallIncrease = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* MediumIncrease = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* LargeIncrease = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* VeryLargeIncrease = new FuzzyRuleConsequent();

    VeryLargeDecrease->addOutput(&VL_D);
    LargeDecrease->addOutput(&L_D);
    MediumDecrease->addOutput(&M_D);
    SmallDecrease->addOutput(&S_D);
    Keep->addOutput(&K);
    SmallIncrease->addOutput(&S_I);
    MediumIncrease->addOutput(&M_I);
    LargeIncrease->addOutput(&L_I);
    VeryLargeIncrease->addOutput(&VL_I);
    
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifLargeNegativeAndQuicklyDecreasing, VeryLargeIncrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifLargeNegativeAndSlowlyDecreasing, LargeIncrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifLargeNegativeAndNotMoving, MediumIncrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifLargeNegativeAndSlowlyIncreasing, SmallIncrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifLargeNegativeAndQuicklyIncreasing, Keep));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifSmallNegativeAndQuicklyDecreasing, LargeIncrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifSmallNegativeAndSlowlyDecreasing, MediumIncrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifSmallNegativeAndNotMoving, SmallIncrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifSmallNegativeAndSlowlyIncreasing, Keep));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifSmallNegativeAndQuicklyIncreasing, SmallDecrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifZeroAndQuicklyDecreasing, MediumIncrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifZeroAndSlowlyDecreasing, SmallIncrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifZeroAndNotMoving, Keep));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifZeroAndSlowlyIncreasing, SmallDecrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifZeroAndQuicklyIncreasing, MediumDecrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifSmallPositiveAndQuicklyDecreasing, SmallIncrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifSmallPositiveAndSlowlyDecreasing, Keep));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifSmallPositiveAndNotMoving, SmallDecrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifSmallPositiveAndSlowlyIncreasing, MediumDecrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifSmallPositiveAndQuicklyIncreasing, LargeDecrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifLargePositiveAndQuicklyDecreasing, Keep));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifLargePositiveAndSlowlyDecreasing, SmallDecrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifLargePositiveAndNotMoving, MediumDecrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifLargePositiveAndSlowlyIncreasing, LargeDecrease));
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifLargePositiveAndQuicklyIncreasing, VeryLargeDecrease));
  } 
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
  Serial.print(M_D.getPertinence());
  Serial.print(' ');
  Serial.print(S_D.getPertinence());
  Serial.print(' ');
  Serial.print(K.getPertinence());
  Serial.print(' ');
  Serial.print(S_I.getPertinence());
  Serial.print(' ');
  Serial.print(M_I.getPertinence());
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
   			     (int)throttle, MIN_POWER, MAX_POWER);

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

bool Controller::getStatus() {
  return fabs(*_sensor - target) <= maxError;
}
