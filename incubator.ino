#include <Sensirion.h>
#include <LiquidCrystal.h>
#include "HumiditySensor.h"
#include "TemperatureSensor.h"
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

unsigned long int tick_counter = 0;

LiquidCrystal lcd = LiquidCrystal(RSPIN, ENABLEPIN, D4PIN, D5PIN, D6PIN, D7PIN); 
Sensirion shtxx = Sensirion(DATAPIN, SCKPIN);
TemperatureSensor temperatureSensor;; 
HumiditySensor humiditySensor;

Controller humidityController((Sensor *)&humiditySensor, HUMIDIFIERPIN);
Controller temperatureController((Sensor *)&temperatureSensor, HEATERPIN);

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

void setup (void){
  Serial.begin(9600);
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

  humidityController.restore(HUMIDADDR);
  temperatureController.restore(TEMPADDR);

  eggTurnerTimer.init(EGGTURNERPIN);
  serialComm.init((Sensor *)&temperatureSensor, (Sensor *)&humiditySensor, 
  		  &eggTurnerTimer, &temperatureController, &humidityController);
  eggTurnerTimer.restore(TURNERADDR);

  lcd.begin(16,2);
  statusScreen.init(&lcd, &temperatureSensor,
   		    &humiditySensor, &eggTurnerTimer);
  statusScreen.activate(true);
  temperatureScreen.init(&lcd, "temp", &temperatureController);
  temperatureScreen.activate(false);
  humidityScreen.init(&lcd, "RH", &humidityController);
  humidityScreen.activate(false);
  timerScreen.init(&lcd, "Turner", &eggTurnerTimer);
  timerScreen.activate(false);
}

void loop (void) {
  unsigned long int lastPush = millis();
  uint8_t activeScreen = 0;
  const float step = 0.1;
  float temperature;
  float humidity;
  bool measActive = false;
  uint8_t measType = TEMP;
  bool measReady = false;
  unsigned int rawData;
  unsigned long int now;
  bool lastSelectRead = HIGH;
  bool selectRead;

  // Serial.println("Init");

  analogWrite(FANPIN, 255);

  while (true) {
    now = millis();
    if ((now - lastPush) > 10000){
      if (activeScreen != 0) {
    	screen[activeScreen]->activate(false);
    	activeScreen = 0;
    	screen[activeScreen]->activate(true);
      }
    }

    selectRead = digitalRead(SELECTBUTTON);
    if (!selectRead && lastSelectRead) {
      lastPush = now;
      screen[activeScreen]->activate(false);
      activeScreen = (activeScreen + 1) % NUMBEROFSCREENS;
      screen[activeScreen]->activate(true);
    }

    if (!digitalRead(INCREASEBUTTON)){
      lastPush = now;
      screen[activeScreen]->modify(step);
    }

    if (!digitalRead(DECREASEBUTTON)){
      lastPush = now;
      screen[activeScreen]->modify(-1 * step);
    }

    if (!(tick_counter % 10)) {
      eggTurnerTimer.check();
    }

    if (!(tick_counter % 100)) {
      if (!measActive) {
	measActive = true;
	measType = TEMP;
	shtxx.meas(measType, &rawData, NONBLOCK);
      } else {
	Serial.println("A reading shouldn't be active!");
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
	Serial.println("problem reading data");
	shtxx.meas(measType, &rawData, NONBLOCK);
	break;
      }
    }

    if (measReady) {
      measReady = false;
      temperatureSensor.update(temperature);
      humiditySensor.update(humidity);
      temperatureController.control();
      humidityController.control();
      serialComm.refresh();
    }
    
    if (!(tick_counter % 100)){
      temperatureController.save(TEMPADDR);
      humidityController.save(HUMIDADDR);
      eggTurnerTimer.save(TURNERADDR);
    }
    
    lastSelectRead = selectRead;
    tick_counter++;
    delay(PERIOD-(millis()-now));
  }

}
