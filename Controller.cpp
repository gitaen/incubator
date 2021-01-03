#include <Arduino.h>
#include <EEPROM.h>
#include "Controller.h"

#define MIN_POWER 0
#define MAX_POWER 255

#define NUM_RULES 25

//#define DEBUG

Fuzzy *Controller::fuzzy = NULL;

FuzzyInput Controller::errorInput = FuzzyInput(1);
FuzzySet Controller::L_N = FuzzySet(-2, -2, -1, -0.5);
FuzzySet Controller::S_N = FuzzySet(-1, -0.5, -0.3, 0);
FuzzySet Controller::Z = FuzzySet(-0.3, 0, 0, 0.3);
FuzzySet Controller::S_P = FuzzySet(0, 0.3, 0.5, 1);
FuzzySet Controller::L_P = FuzzySet(0.5, 1, 2, 2);

FuzzyInput Controller::errorDeltaInput = FuzzyInput(2);
FuzzySet Controller::L_NA = FuzzySet(-0.05, -0.05, -0.0075, -0.0025);
FuzzySet Controller::S_NA = FuzzySet(-0.0075, -0.0025, -0.00125, 0);
FuzzySet Controller::Z_A = FuzzySet(-0.00125, 0, 0, 0.00125);
FuzzySet Controller::S_PA = FuzzySet(0, 0.00125, 0.0025, 0.0075);
FuzzySet Controller::L_PA = FuzzySet(0.0025, 0.0075, 0.05, 0.05);

FuzzyOutput Controller::adjust = FuzzyOutput(1);
FuzzySet Controller::L_D = FuzzySet(-16, -16, -16, -12);
FuzzySet Controller::M_D = FuzzySet(-16, -12, -8, -4);
FuzzySet Controller::S_D = FuzzySet(-8, -4, -4, 0);
FuzzySet Controller::K = FuzzySet(-4, 0, 0, 4);
FuzzySet Controller::S_I = FuzzySet(0, 4, 4, 8);
FuzzySet Controller::M_I = FuzzySet(4, 8, 12, 16);
FuzzySet Controller::L_I = FuzzySet(12, 16, 16, 16);

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
    adjust.addFuzzySet(&M_D);
    adjust.addFuzzySet(&S_D);
    adjust.addFuzzySet(&K);
    adjust.addFuzzySet(&S_I);
    adjust.addFuzzySet(&M_I);
    adjust.addFuzzySet(&L_I);
    fuzzy->addFuzzyOutput(&adjust);

    FuzzyRuleAntecedent* ifLargeNegative = new FuzzyRuleAntecedent();
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
    FuzzyRuleAntecedent* ifLargePositive = new FuzzyRuleAntecedent();

    ifLargeNegative->joinSingle(&L_N);
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
    ifLargePositive->joinSingle(&L_P);

    FuzzyRuleConsequent* LargeDecrease = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* MediumDecrease = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* SmallDecrease = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* Keep = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* SmallIncrease = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* MediumIncrease = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* LargeIncrease = new FuzzyRuleConsequent();

    LargeDecrease->addOutput(&L_D);
    MediumDecrease->addOutput(&M_D);
    SmallDecrease->addOutput(&S_D);
    Keep->addOutput(&K);
    SmallIncrease->addOutput(&S_I);
    MediumIncrease->addOutput(&M_I);
    LargeIncrease->addOutput(&L_I);

    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifLargeNegative, LargeIncrease));
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
    fuzzy->addFuzzyRule(new FuzzyRule(i++, ifLargePositive, LargeDecrease));
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
    if (i%5) {
      Serial.print(' ');
    } else {
      Serial.println();
    }
  }
  Serial.println();

  Serial.print(VL_D.getPertinence());
  Serial.print(' ');
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
  Serial.print(L_I.getPertinence());
  Serial.print(' ');
  Serial.println(VL_I.getPertinence());
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
  unsigned int i;
  byte *b = (byte *)&target;

  if (modified) {
    for (i=0; i<sizeof(float); i++) {
      EEPROM.write(address+i, *b++);
    }
  }
  
  modified = false;
}

void Controller::restore(int address) {
  unsigned int i;
  byte *b = (byte *)&target;

  for (i=0; i<sizeof(float); i++) {
    *b++ = EEPROM.read(address+i);
  }
  
  modified = false;
}

bool Controller::getStatus() {
  return fabs(*_sensor - target) <= maxError;
}
