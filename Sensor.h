#ifndef _SENSOR_
#define _SENSOR_

#include "Subject.h"

class Sensor: public Subject
{
  public:
     void update(float);
    float getMeasurement(void);

 private:
    float value = 0;
};

#endif
