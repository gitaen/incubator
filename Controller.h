#ifndef _CONTROLLER_
#define _CONTROLLER_

#include <FuzzyRule.h>
#include <FuzzyComposition.h>
#include <Fuzzy.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzyOutput.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzySet.h>
#include <FuzzyRuleAntecedent.h>

class Controller
{
private:
  float *_sensor;
  uint8_t pin;
  uint8_t power;
  float target;
  float lastError;
  unsigned long lastTime;
  uint8_t modified;
  static Fuzzy *fuzzy;
  static FuzzySet L_N;
  static FuzzySet S_N;
  static FuzzySet Z;
  static FuzzySet S_P;
  static FuzzySet L_P;
  static FuzzyInput errorInput;
  static FuzzySet L_NA;
  static FuzzySet S_NA;
  static FuzzySet Z_A;
  static FuzzySet S_PA;
  static FuzzySet L_PA;
  static FuzzyInput errorDeltaInput;
  static FuzzySet L_D;
  static FuzzySet M_D;
  static FuzzySet S_D;
  static FuzzySet K;
  static FuzzySet S_I;
  static FuzzySet M_I;
  static FuzzySet L_I;
  static FuzzyOutput adjust;

public:
  Controller (float *sensor, uint8_t pinNumber);
  void setTarget(float targetValue);
  float getTarget(void);
  uint8_t getPower(void);
  float *getSensor();
  void control(void);
  void save(int address);
  void restore(int address);
};

#endif
