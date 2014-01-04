#include "Subject.h"

void Subject::attach(Observer *observer) {
  ObserverList *aux;
  ObserverList *iter;

  // aux = (ObserverList *) malloc(sizeof(ObserverList));
  aux = new ObserverList();

  aux->observer 	= observer;
  aux->next 		= 0;
  
  if(this->observers == 0){
    this->observers = aux;
  }else{
    iter = this->observers;
    while (iter->next != 0) {
      iter = iter->next;
    }
    iter->next = aux;
  }
}

void Subject::notify(void) {
  ObserverList *iter;
  iter = this->observers;
  while (iter != 0) {
    iter->observer->update(this);
    iter = iter->next;
  }
}
