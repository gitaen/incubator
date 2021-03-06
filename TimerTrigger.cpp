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
  problem = false;
  steps=0;
}


void TimerTrigger::check(void) {
  if (active) {
    updateTimeLeft();
    if (timeLeft <= 0) {
      if (!onState) {
	onState = true;
	timeLeft = OFFTIME;
	lastMillis = millis();
	analogWrite(pin, 255);
      } else {
	// It didn't finish turning within the expected time
	problem = true;
      }
    }
  }
}

void TimerTrigger::step(void) {
  steps++;
  if (steps >= MAXSTEPS) {
	onState = false;
	problem = false;
	steps=0;
	analogWrite(pin, 0);
  }
}

bool TimerTrigger::getOnState(void) {
  return onState;
}

void TimerTrigger::activate (bool on) {
  if (on != active) {
    active = on;
    problem = false;
    modified = true;
  }
  if (!active) {
    analogWrite(pin, 0);
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
}

bool TimerTrigger::getStatus() {
  return !problem;
}

bool TimerTrigger::isActive (void){
  return active;
}


unsigned long TimerTrigger::getTimeLeft(void) {
  return timeLeft;
}

void TimerTrigger::updateTimeLeft(void) {
  unsigned long elapsedSeconds;

  if (millis() < lastMillis) { // millis rolled over
    elapsedSeconds = ((34359737 - lastMillis) + millis())/1000;
  }
  else {
    elapsedSeconds = (millis() - lastMillis)/1000;
  }
  
  if (onState) {
    timeLeft = MAXONTIME - elapsedSeconds;
  } else {
    timeLeft = OFFTIME - elapsedSeconds;
  }
}

