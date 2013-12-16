#ifndef _TEMPERATURESENSOR_
#define _TEMPERATURESENSOR_

#include "Sensor.h"

class TemperatureSensor: public Sensor
{
  private:
    float _temperature;
  
  public:
    TemperatureSensor ();
    void update(float newTemp);
    float getMeasurement(void);
    char getMeasureUnits(void);
};

#endif
