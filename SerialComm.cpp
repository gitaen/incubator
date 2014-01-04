#include <Arduino.h>
#include <HardwareSerial.h>
#include <string.h>
#include "SerialComm.h"

void SerialComm::init(TimerTrigger *eggTurner, Controller *tempController, 
		      Controller *humidController)
{
  _eggTurner = eggTurner;
  _tempController = tempController;
  _humidController = humidController;
}

void SerialComm::refresh()
{
  Serial.print(_tempController->getSensor()->getMeasurement());
  Serial.print(' ');
  Serial.print(_humidController->getSensor()->getMeasurement());
  Serial.print(' ');
  Serial.print(_eggTurner->getOnState());
  Serial.print(' ');
  Serial.print(_eggTurner->isActive());
  Serial.print(' ');
  Serial.print(_eggTurner->getTimeLeft());
  Serial.print(' ');
  Serial.print(_tempController->getTarget());
  Serial.print(' ');
  Serial.print(_tempController->getPower());
  Serial.print(' ');
  Serial.print(_humidController->getPower());
  Serial.print(' ');
  Serial.print(_humidController->getTarget());
  Serial.print(' ');
  Serial.print(millis());
  Serial.println();
}
