#ifndef _SCREEN_
#define _SCREEN_

class Screen
{
protected:
  bool activated;
  
public:
  
  virtual void activate(bool act) {activated = act;};
  bool isActive(void) {return activated;};
  virtual void modify(float step) {};
  
};

#endif
