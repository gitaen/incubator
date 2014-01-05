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
  Serial.print(millis());
  Serial.print(' ');
  Serial.print(*_tempController->getSensor());
  Serial.print(' ');
  Serial.print(_tempController->getTarget());
  Serial.print(' ');
  Serial.print(_tempController->getPower());
  Serial.print(' ');
  Serial.print(*_humidController->getSensor());
  Serial.print(' ');
  Serial.print(_humidController->getTarget());
  Serial.print(' ');
  Serial.print(_humidController->getPower());
  Serial.print(' ');
  Serial.print(_eggTurner->getOnState());
  Serial.print(' ');
  Serial.print(_eggTurner->isActive());
  Serial.print(' ');
  Serial.print(_eggTurner->getTimeLeft());
  Serial.println();
  Serial.flush();
}
