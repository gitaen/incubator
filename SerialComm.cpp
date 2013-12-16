//#include <string.h>
//#include <stdio.h>
#include <HardwareSerial.h>
#include <string.h>
#include "SerialComm.h"
#include "floatToString.h"

void SerialComm::init(Sensor *temp, Sensor *humidity, TimerTrigger *eggTurner)
{
  _temperatureSensor = temp;
  _humiditySensor = humidity;
  _eggTurner = eggTurner;

  _temperatureSensor->attach(this);
  _humiditySensor->attach(this);
  _eggTurner->attach(this);
}


void SerialComm::update(Subject* subject)
{
  float tempFloat;
  char str[7];

  if (subject == _temperatureSensor) {
    tempFloat = _temperatureSensor->getMeasurement();
    str[0] = 'T';
    floatToString(&str[1], tempFloat, 2, 0, false);
  }
  else if (subject == _humiditySensor) {
    tempFloat = _humiditySensor->getMeasurement();
    str[0] = 'H';
    floatToString(&str[1], tempFloat, 2, 0, false);
  }
  else if (subject == _eggTurner) {
    if (_eggTurner->getOnState())
      strcpy(str,"EON");
    else
      strcpy(str,"EOFF");
  }

  Serial.println(str);
}
