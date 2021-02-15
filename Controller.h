#ifndef _CONTROLLER_
#define _CONTROLLER_

#include <PID_v1.h>
#include <PID_AutoTune_v0.h>


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
  bool autoTuneEnabled;
  PID_ATune autoTune;

  void runAutoTune(void);
  void runController(void);

public:
  Controller (float *sensor, uint8_t pinNumber, unsigned int sampleTime);
  void enableAutoTuning(void);
  void setTarget(float targetValue);
  void setMaxError(float max);
  float getTarget(void);
  uint8_t getPower(void);
  float *getSensor();
  void control(void);
  void save(int address);
  void restore(int address);
  bool getStatus();
  bool isAutoTuningEnabled(void);
  size_t getConfigSize(void);
  double getKp(void);
  double getKi(void);
  double getKd(void);
};

#endif
