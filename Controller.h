#ifndef _CONTROLLER_
#define _CONTROLLER_

#include <PID_v1.h>

class Controller
{
private:
  float *_sensor;
  double sensorDouble;
  uint8_t pin;
  double power;
  double target;
  double maxError;
  uint8_t modified;
  PID pidControl;

public:
  Controller (float *sensor, uint8_t pinNumber);
  void setTarget(float targetValue);
  void setMaxError(float max);
  float getTarget(void);
  uint8_t getPower(void);
  float *getSensor();
  void control(void);
  void save(int address);
  void restore(int address);
  bool getStatus();
};

#endif
