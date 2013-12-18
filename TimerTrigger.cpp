#include <Arduino.h>
#include <EEPROM.h>
#include "TimerTrigger.h"

void TimerTrigger::init (uint8_t pinNumber) {
  pin = pinNumber;
  onState = false;
  timeLeft = OFFTIME;
  active = true;
  lastMillis = millis();
  modified = false;
}


void TimerTrigger::check(void) {
  if (active) {
    updateTimeLeft();
    if (timeLeft == 0) {
      if (onState && OFFTIME != 0) {
	onState = false;
	timeLeft = OFFTIME;
	lastMillis = millis();
	digitalWrite(pin, LOW);
	notify();
      }
      else if (!onState && ONTIME != 0) {
	onState = true;
	timeLeft = ONTIME;
	lastMillis = millis();
	analogWrite(pin, 127);
	notify();
      }
    }
  }
}

bool TimerTrigger::getOnState(void) {
  return onState;
}

void TimerTrigger::activate (bool on) {
  if (on != active) {
    active = on;
    modified = true;
    notify();
  }
}


void TimerTrigger::save(int address) {
  if (modified){
    EEPROM.write(address, active);
  }
  
  modified = false;
}

void TimerTrigger::restore(int address) {
  active = EEPROM.read(address);
  modified = false;
  notify();
}

bool TimerTrigger::isActive (void){
  return active;
}


unsigned long TimerTrigger::getTimeLeft(void) {
  return timeLeft;
}

void TimerTrigger::updateTimeLeft(void) {
  unsigned long elapsedSeconds;
  unsigned long time;

  if (onState)
    time = ONTIME;
  else
    time = OFFTIME;
  
  if (millis() < lastMillis) { // millis rolled over
    elapsedSeconds = ((34359737 - lastMillis) + millis())/1000;
  }
  else {
    elapsedSeconds = (millis() - lastMillis)/1000;
  }
  
  if ((time - elapsedSeconds) != timeLeft) {
    timeLeft = (time - elapsedSeconds);
    notify();
  }
}

