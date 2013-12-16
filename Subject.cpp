#include "Subject.h"
//#include <HardwareSerial.h>

Subject::Subject() {
//  Serial.println("Subject::Subject");
  length = 0;
}

void Subject::attach(Observer *observer) {
//  Serial.println("Subject::attach");
  observers[length] = observer;
  length++;
}

//void Subject::detach(Observer *observer) {
//  uint8_t i=0;

//  while ((i<length) && (observers[i] != observer)) {
//    i++;
//  }

//  if (i != length) {
//    while (i<(length-1)) {
//      observers[i] = observers[i+1];
//      i++;
//    }
//    length--;
//  }
//}

void Subject::notify(void) {
//  Serial.println("Subject::notify");
//  Serial.println(length, DEC);
  uint8_t i;

  for (i=0;i<length;i++) {
    observers[i]->update(this);
  }
}
