#ifndef _SUBJECT_
#define _SUBJECT_

#define MAX_OBSERVERS 3

#include <stdint.h>
#include "Observer.h"

class Subject 
{
public:
  void attach(Observer *observer);
//  void detach(Observer *observer);
  void notify(void);
  Subject ();
private:
  Observer* observers[MAX_OBSERVERS];
  uint8_t length;
};

#endif
