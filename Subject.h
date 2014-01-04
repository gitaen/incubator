#ifndef _SUBJECT_
#define _SUBJECT_


#include <stdint.h>
#include "Observer.h"

struct ObserverList {
  Observer *observer;
  struct ObserverList* next;
};

class Subject 
{
public:
  void attach(Observer *observer);
//  void detach(Observer *observer);
  void notify(void);
private:
  struct ObserverList* observers = 0;
};

#endif
