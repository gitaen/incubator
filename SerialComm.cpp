//#include <string.h>
//#include <stdio.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include <string.h>
#include "SerialComm.h"

void SerialComm::init(Sensor *temp, Sensor *humidity, TimerTrigger *eggTurner, 
		      Controller *tempController, Controller *humidController)
{
  _temperatureSensor = temp;
  _humiditySensor = humidity;
  _eggTurner = eggTurner;
  _tempController = tempController;
  _humidController = humidController;

  _temperatureSensor->attach(this);
  _humiditySensor->attach(this);
  _eggTurner->attach(this);
  _tempController->attach(this);
  _humidController->attach(this);
  update(tempController);
  update(humidController);
}


void SerialComm::update(Subject* subject)
{
  if (subject == _temperatureSensor) {
    temp = _temperatureSensor->getMeasurement();
  } else if (subject == _humiditySensor) {
    humid = _humiditySensor->getMeasurement();
  } else if (subject == _eggTurner) {
    turnerState = _eggTurner->getOnState();
    turnerActive = _eggTurner->isActive();
    turnerRemTime = _eggTurner->getTimeLeft();
  } else if (subject == _tempController) {
    tempPower = _tempController->getPower();
    tempTarget = _tempController->getTarget();
  } else if (subject == _humidController) {
    humidPower = _humidController->getPower();
    humidTarget = _humidController->getTarget();
  }
}

void SerialComm::refresh()
{
  Serial.print(temp);
  Serial.print(' ');
  Serial.print(humid);
  Serial.print(' ');
  Serial.print(turnerState);
  Serial.print(' ');
  Serial.print(turnerActive);
  Serial.print(' ');
  Serial.print(turnerRemTime);
  Serial.print(' ');
  Serial.print(tempTarget);
  Serial.print(' ');
  Serial.print(tempPower);
  Serial.print(' ');
  Serial.print(humidTarget);
  Serial.print(' ');
  Serial.print(humidPower);
  Serial.print(' ');
  Serial.print(millis());
  Serial.println();
}
