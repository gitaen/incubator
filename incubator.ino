#include <avr/wdt.h>
#include <Wire.h>
#include <SHTSensor.h>
#include <LiquidCrystal.h>
#include "Controller.h"
#include "StatusScreen.h"
#include "ControllerScreen.h"
#include "TimerScreen.h"
#include "TimerTrigger.h"
#include "Controller.h"
#include "SerialComm.h"

#define PERIOD 200

#define NUMBEROFSCREENS 4
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
#define D6PIN 2
#define D7PIN 3
#define REDLEDPIN 12
#define GREENLEDPIN 13

#define TEMPADDR 0
#define HUMIDADDR 4
#define TURNERADDR 8

uint8_t tick_counter = 0;

LiquidCrystal lcd = LiquidCrystal(RSPIN, ENABLEPIN, D4PIN, D5PIN, D6PIN, D7PIN); 
SHTSensor shtxx;

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
  Wire.begin();
  shtxx.init();
  shtxx.setAccuracy(SHTSensor::SHT_ACCURACY_HIGH);
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
  pinMode(REDLEDPIN, OUTPUT);
  pinMode(GREENLEDPIN, OUTPUT);

  humidityController->restore(HUMIDADDR);
  humidityController->setMaxError(2);
  temperatureController->restore(TEMPADDR);
  temperatureController->setMaxError(0.2);

  eggTurnerTimer.init(EGGTURNERPIN);
  serialComm.init(&eggTurnerTimer, temperatureController, humidityController);
  eggTurnerTimer.restore(TURNERADDR);

  lcd.begin(16,2);
  statusScreen.init(&lcd, &temperature,
   		    &humidity, &eggTurnerTimer);
  temperatureScreen.init(&lcd, F("temp"), temperatureController, 'C');
  humidityScreen.init(&lcd, F("RH"), humidityController, '%');
  timerScreen.init(&lcd, F("Turner"), &eggTurnerTimer);

  // temperatureController->setTarget(37.7);
  // humidityController->setTarget(55);
  // while (!Serial.available()) {
  //   delay (1000);
  // }
  wdt_enable(WDTO_8S);
}

void loop (void) {
  unsigned long int lastPush = millis();
  uint8_t activeScreen = 0;
  const float step = 0.1;
  unsigned long int now;
  bool lastSelectRead = digitalRead(SELECTBUTTON);
  bool selectRead;
  bool lastEncRead = digitalRead(ENCODERPIN);
  bool encRead;
  bool problem = false;

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

    if (!(tick_counter % 5)) {
      eggTurnerTimer.check();
      screen[activeScreen]->refresh();
    }

    if (!(tick_counter % 50)) {
      if (shtxx.readSample()) {
        temperature = shtxx.getTemperature();
        humidity = shtxx.getHumidity();
        temperatureController->control();
        humidityController->control();
        screen[activeScreen]->refresh();
      } else {
	Serial.println(88888888);
      }
    } 

    if (!(tick_counter % 50)){
      temperatureController->save(TEMPADDR);
      humidityController->save(HUMIDADDR);
      eggTurnerTimer.save(TURNERADDR);
    }
    
    serialComm.check();
    problem = !temperatureController->getStatus() || !humidityController->getStatus()
      || !eggTurnerTimer.getStatus();

    if (problem) {
      digitalWrite(GREENLEDPIN, LOW);
      digitalWrite(REDLEDPIN, HIGH);
    } else {
      digitalWrite(GREENLEDPIN, HIGH);
      digitalWrite(REDLEDPIN, LOW);
    }

    lastSelectRead = selectRead;
    tick_counter = (tick_counter + 1) % 100;
    if ((millis()-now) < PERIOD) {
      delay(PERIOD-(millis()-now));
    }

    wdt_reset();
  }

}
