#include <Sensirion.h>
#include <LiquidCrystal.h>
#include <MemoryFree.h>
#include "Controller.h"
#include "StatusScreen.h"
#include "ControllerScreen.h"
#include "TimerScreen.h"
#include "TimerTrigger.h"
#include "Controller.h"
#include "SerialComm.h"

#define PERIOD 100

#define NUMBEROFSCREENS 4
#define SCKPIN 3
#define DATAPIN 2
#define FANPIN 5
#define HUMIDIFIERPIN 9
#define HEATERPIN 10
#define EGGTURNERPIN 6
#define ENCODERPIN 4
#define SELECTBUTTON 7
#define DECREASEBUTTON 8
#define INCREASEBUTTON 11
#define RSPIN A0
#define ENABLEPIN A1
#define D4PIN A3
#define D5PIN A2
#define D6PIN A5
#define D7PIN A4

#define TEMPADDR 0
#define HUMIDADDR 4
#define TURNERADDR 8

uint8_t tick_counter = 0;

LiquidCrystal lcd = LiquidCrystal(RSPIN, ENABLEPIN, D4PIN, D5PIN, D6PIN, D7PIN); 
Sensirion shtxx = Sensirion(DATAPIN, SCKPIN);

Controller *humidityController;
Controller *temperatureController;

TimerTrigger eggTurnerTimer;

SerialComm serialComm;

StatusScreen statusScreen;

ControllerScreen temperatureScreen;
ControllerScreen humidityScreen;
TimerScreen timerScreen;

Screen *screen[NUMBEROFSCREENS] = {&statusScreen, 
				   &temperatureScreen, 
				   &humidityScreen,
				   &timerScreen};

float temperature;
float humidity;

void setup (void){
  Serial.begin(115200);
  humidityController = new Controller(&humidity, HUMIDIFIERPIN);
  temperatureController = new Controller(&temperature, HEATERPIN);

  pinMode(SELECTBUTTON, INPUT);
  digitalWrite(SELECTBUTTON, HIGH);
  pinMode(INCREASEBUTTON, INPUT);
  digitalWrite(INCREASEBUTTON, HIGH);
  pinMode(DECREASEBUTTON, INPUT);
  digitalWrite(DECREASEBUTTON, HIGH);
  pinMode(ENCODERPIN, INPUT);
  digitalWrite(ENCODERPIN, HIGH);

  pinMode(FANPIN, OUTPUT);
  pinMode(HUMIDIFIERPIN, OUTPUT);
  pinMode(HEATERPIN, OUTPUT);
  pinMode(EGGTURNERPIN, OUTPUT);

  humidityController->restore(HUMIDADDR);
  temperatureController->restore(TEMPADDR);

  eggTurnerTimer.init(EGGTURNERPIN);
  serialComm.init(&eggTurnerTimer, temperatureController, humidityController);
  eggTurnerTimer.restore(TURNERADDR);

  lcd.begin(16,2);
  statusScreen.init(&lcd, &temperature,
   		    &humidity, &eggTurnerTimer);
  temperatureScreen.init(&lcd, F("temp"), temperatureController, 'C');
  humidityScreen.init(&lcd, F("RH"), humidityController, '%');
  timerScreen.init(&lcd, F("Turner"), &eggTurnerTimer);

  // while (!Serial.available()) {
  //   delay (1000);
  // }
}

void loop (void) {
  unsigned long int lastPush = millis();
  uint8_t activeScreen = 0;
  const float step = 0.1;
  bool measActive = false;
  uint8_t measType = TEMP;
  bool measReady = false;
  unsigned int rawData;
  unsigned long int now;
  bool lastSelectRead = digitalRead(SELECTBUTTON);
  bool selectRead;
  bool lastEncRead = digitalRead(ENCODERPIN);
  bool encRead;

  // Serial.println("Init");

  analogWrite(FANPIN, 255);

  while (true) {
    now = millis();
    if ((now - lastPush) > 10000){
      if (activeScreen != 0) {
    	activeScreen = 0;
	screen[activeScreen]->refresh();
      }
    }

    selectRead = digitalRead(SELECTBUTTON);
    if (!selectRead && lastSelectRead) {
      lastPush = now;
      activeScreen = (activeScreen + 1) % NUMBEROFSCREENS;
      screen[activeScreen]->refresh();
    }

    if (!digitalRead(INCREASEBUTTON)){
      lastPush = now;
      screen[activeScreen]->modify(step);
      screen[activeScreen]->refresh();
    }

    if (!digitalRead(DECREASEBUTTON)){
      lastPush = now;
      screen[activeScreen]->modify(-1 * step);
      screen[activeScreen]->refresh();
    }

    encRead = digitalRead(ENCODERPIN);
    if (encRead != lastEncRead) {
      lastEncRead = encRead;
      eggTurnerTimer.step();
    }

    if (!(tick_counter % 10)) {
      eggTurnerTimer.check();
      screen[activeScreen]->refresh();
    }

    if (!(tick_counter % 100)) {
      if (!measActive) {
	measActive = true;
	measType = TEMP;
	shtxx.meas(measType, &rawData, NONBLOCK);
      } else {
	Serial.println(88888888);
      }
    } 

    if (measActive && shtxx.measRdy()) {
      switch(shtxx.measRdy()) {
      case S_Meas_Rdy:
	if (measType == TEMP) {
	  temperature = shtxx.calcTemp(rawData);
	  measType = HUMI;
	  shtxx.meas(measType, &rawData, NONBLOCK);
	} else {
	  humidity = shtxx.calcHumi(rawData, temperature);
	  measActive = false;
	  measReady = true;
	}
	break;
      case S_Err_CRC:
	Serial.println(99999999);
	shtxx.meas(measType, &rawData, NONBLOCK);
	break;
      }
    }

    if (measReady) {
      measReady = false;
      temperatureController->control();
      humidityController->control();
      screen[activeScreen]->refresh();
      serialComm.refresh();
    }
    
    if (!(tick_counter % 100)){
      temperatureController->save(TEMPADDR);
      humidityController->save(HUMIDADDR);
      eggTurnerTimer.save(TURNERADDR);
    }
    
    lastSelectRead = selectRead;
    tick_counter = ++tick_counter % 100;
    delay(PERIOD-(millis()-now));
  }

}
