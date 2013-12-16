#ifndef _SENSOR_
#define _SENSOR_

#include "Subject.h"

class Sensor: public Subject
{
  public:
    Sensor () {};
    virtual void update(float) {};
    virtual float getMeasurement(void) {};
    virtual char getMeasureUnits(void) {};
};

#endif
