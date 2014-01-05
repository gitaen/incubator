#ifndef _TIMERTRIGGER_
#define _TIMERTRIGGER_

#define MAXSTEPS 6
#define OFFTIME 7200

class TimerTrigger {
private:
  unsigned long lastMillis;
  long timeLeft;
  uint8_t pin;
  bool onState;
  bool active;
  uint8_t modified;
  uint8_t steps;

public:
  void init (uint8_t pinNumber);
  void check(void);
  void activate (bool on);
  bool isActive (void);
  bool getOnState(void);
  unsigned long getTimeLeft(void);
  void step();
  void save(int address);
  void restore(int address);

private:
  void updateTimeLeft(void);
};

#endif
