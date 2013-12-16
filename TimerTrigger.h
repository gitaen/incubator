#ifndef _TIMERTRIGGER_
#define _TIMERTRIGGER_

#include "Subject.h"

#define ONTIME 10
#define OFFTIME 7200

class TimerTrigger: public Subject {
private:
  unsigned long lastMillis;
  unsigned long timeLeft;
  uint8_t pin;
  bool onState;
  bool active;
  uint8_t modified;

public:
  void init (uint8_t pinNumber);
  void check(void);
  void activate (bool on);
  bool isActive (void);
  bool getOnState(void);
  unsigned long getTimeLeft(void);
  void save(int address);
  void restore(int address);

private:
  void updateTimeLeft(void);
};

#endif
