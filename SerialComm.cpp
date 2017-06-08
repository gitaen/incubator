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

void SerialComm::check()
{
  char command;
  
  if (Serial.available()) {
    command = Serial.read();
    if (command == 'G') {
      getParameters();
    } else if (command == 'S') {
      setParameters();
    }
  }
}

void SerialComm::getParameters()
{
  Serial.print(millis());
  Serial.print(' ');
  Serial.print(*_tempController->getSensor());
  Serial.print(' ');
  Serial.print(_tempController->getTarget());
  Serial.print(' ');
  Serial.print(_tempController->getPower());
  Serial.print(' ');
  Serial.print(_tempController->getStatus());
  Serial.print(' ');
  Serial.print(*_humidController->getSensor());
  Serial.print(' ');
  Serial.print(_humidController->getTarget());
  Serial.print(' ');
  Serial.print(_humidController->getPower());
  Serial.print(' ');
  Serial.print(_humidController->getStatus());
  Serial.print(' ');
  Serial.print(_eggTurner->getOnState());
  Serial.print(' ');
  Serial.print(_eggTurner->isActive());
  Serial.print(' ');
  Serial.print(_eggTurner->getTimeLeft());
  Serial.print(' ');
  Serial.print(_eggTurner->getStatus());
  Serial.println();
  Serial.flush();
}

void SerialComm::setParameters()
{
  char parameter;
  int intVal;
  float floatVal;
  
  if (Serial.available()) {
    parameter = Serial.read();
    if (parameter == 'T') {
      if ((floatVal = Serial.parseFloat())) {
	_tempController->setTarget(floatVal);
      }
    } else if (parameter == 'H') {
      if ((floatVal = Serial.parseFloat())) {
	_humidController->setTarget(floatVal);
      }
    } else if (parameter == 'E') {
      intVal = Serial.parseInt();
      _eggTurner->activate(intVal);
    }
  }
}
