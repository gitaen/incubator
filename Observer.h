#ifndef _OBSERVER_
#define _OBSERVER_

class Subject;

class Observer
{
public:
  virtual void update(Subject* subject) {};
};

#endif
