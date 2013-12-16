#ifndef _CONTROLLER_
#define _CONTROLLER_

#include "Sensor.h"
#include "Subject.h"

#define NUMBEROFFUZZYSETS 3

//enum FuzzySet {L_N, S_N, Z_E, S_P, L_P}; 
enum FuzzySet {N, Z_E, P}; 

class Controller: public Subject
{
private:
  Sensor *_sensor;
  uint8_t pin;
  uint8_t power;
  float target;
  float lastError;
  unsigned long lastTime;
  float *_errorPoints;
  uint8_t (*_errorFunctions)[NUMBEROFFUZZYSETS];
  float *_deltaPoints;
  uint8_t (*_deltaFunctions)[NUMBEROFFUZZYSETS];
  uint8_t (*_rules)[NUMBEROFFUZZYSETS];
  float *_outputFunction;
  uint8_t modified;

public:
  void setTarget(float targetValue);
  float getTarget(void);
  Sensor *getSensor();
  void init (Sensor *sensor, uint8_t pinNumber,  
	     float errorPoints[NUMBEROFFUZZYSETS],
	     uint8_t errorFunctions[NUMBEROFFUZZYSETS][NUMBEROFFUZZYSETS],
	     float DeltaPoints[NUMBEROFFUZZYSETS],
	     uint8_t deltaFunctions[NUMBEROFFUZZYSETS][NUMBEROFFUZZYSETS],
	     uint8_t rules[NUMBEROFFUZZYSETS][NUMBEROFFUZZYSETS],
	     float outputFunction[NUMBEROFFUZZYSETS]);
  void control(void);
  void save(int address);
  void restore(int address);

private:
  float fuzzify(float error, float points[], uint8_t function[]);
//  void infere(float *errorArray, float *deltaArray, float rulesResultArray[NUMBEROFFUZZYSETS][NUMBEROFFUZZYSETS]);
  float defuzzify(float *errorArray, float *deltaArray);
};

#endif
