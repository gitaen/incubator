#ifndef _TIMERTRIGGER_
#define _TIMERTRIGGER_

#define MAXSTEPS 12
#define OFFTIME 14400
#define MAXONTIME 10

class TimerTrigger {
private:
  unsigned long lastMillis;
  long timeLeft;
  uint8_t pin;
  bool onState;
  bool active;
  bool problem;
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
  bool getStatus();

private:
  void updateTimeLeft(void);
};

#endif
