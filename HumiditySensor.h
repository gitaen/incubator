#ifndef _HUMIDITYSENSOR_
#define _HUMIDITYSENSOR_

#include "Sensor.h"

class HumiditySensor: public Sensor
{
  private:
    float _humidity;
  
  public:
    HumiditySensor ();
    void update(float newHumid);
    float getMeasurement(void);
    char getMeasureUnits(void);
};

#endif
