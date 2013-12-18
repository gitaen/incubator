#include <Sensirion.h>
#include <LiquidCrystal.h>
//#include <stdio.h>
#include "HumiditySensor.h"
#include "TemperatureSensor.h"
#include "Controller.h"
#include "StatusScreen.h"
#include "ControllerScreen.h"
#include "TimerScreen.h"
#include "TimerTrigger.h"
#include "Controller.h"
#include "SerialComm.h"

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
#define D4PIN A2
#define D5PIN A3
#define D6PIN A4
#define D7PIN A5

#define TEMPADDR 0
#define HUMIDADDR 4
#define TURNERADDR 8

uint8_t tick_counter = 0;

LiquidCrystal lcd = LiquidCrystal(RSPIN, ENABLEPIN, D4PIN, D5PIN, D6PIN, D7PIN); 
Sensirion shtxx = Sensirion(DATAPIN, SCKPIN);
TemperatureSensor temperatureSensor;; 
HumiditySensor humiditySensor;

// float errorPoints[NUMBEROFFUZZYSETS] = {-1, 0, 1};
// uint8_t functions[NUMBEROFFUZZYSETS][NUMBEROFFUZZYSETS] =
//   {{1,0,0},
//    {0,1,0},
//    {0,0,1}};

// float deltaPoints[NUMBEROFFUZZYSETS] = {-0.25, 0, 0.25};

// uint8_t rules[NUMBEROFFUZZYSETS][NUMBEROFFUZZYSETS] = 
//   {{P, P, Z_E},  // error = N
//    {P, Z_E, N},  // error = Z_E
//    {Z_E, N, N}};  // error = P
// float outputFunction[NUMBEROFFUZZYSETS] = {-2, 0, 2};

 float errorPoints[NUMBEROFFUZZYSETS] = {-2, -1, 0, 1, 2};
 uint8_t functions[NUMBEROFFUZZYSETS][NUMBEROFFUZZYSETS] =
   {{1,0,0,0,0},
    {0,1,0,0,0},
    {0,0,1,0,0},
    {0,0,0,1,0},
    {0,0,0,0,1}};

 float deltaPoints[NUMBEROFFUZZYSETS] = {-0.05, -0.02, 0, 0.02, 0.05};

 uint8_t rules[NUMBEROFFUZZYSETS][NUMBEROFFUZZYSETS] = 
   {{L_P, L_P, L_P, S_P, Z_E},  // error = L_N
    {L_P, L_P, S_P, Z_E, S_N},  // error = S_N
    {L_P, S_P, Z_E, S_N, L_N},  // error = Z_E
    {S_P, Z_E, S_N, L_N, L_N},  // error = S_P
    {Z_E, S_N, L_N, L_N, L_N}}; // error = L_P
 float outputFunction[NUMBEROFFUZZYSETS] = {-3, -1, 0, 1, 3};

Controller humidityController;
Controller temperatureController;

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

  humidityController.init((Sensor *)&humiditySensor, HUMIDIFIERPIN,
  		    errorPoints, functions, 
  		    deltaPoints, functions, 
                    rules, outputFunction);
  humidityController.restore(HUMIDADDR);


  temperatureController.init((Sensor *)&temperatureSensor, HEATERPIN,
    		    errorPoints, functions, 
  		    deltaPoints, functions, 
                  rules, outputFunction);
  temperatureController.restore(TEMPADDR);

  eggTurnerTimer.init(EGGTURNERPIN);
  serialComm.init((Sensor *)&temperatureSensor, (Sensor *)&humiditySensor, 
  		  &eggTurnerTimer);
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

  temperatureController.setTarget(37.7);
  humidityController.setTarget(55);
}

void loop (void) {
  unsigned long int lastPush = millis();
  uint8_t activeScreen = 0;
  float step = 0.1;
  float temperature;
  float humidity;
  bool measActive = false;
  uint8_t measType = TEMP;
  bool measReady = false;
  unsigned int rawData;
  unsigned long int now;

  Serial.println("Init");

  analogWrite(FANPIN, 255);

  while (true) {
    now = millis();
    if ((now - lastPush) > 5000){
      if (activeScreen != 0) {
    	screen[activeScreen]->activate(false);
    	activeScreen = 0;
    	screen[activeScreen]->activate(true);
      }
    }

    if (!digitalRead(SELECTBUTTON)){
      lastPush = millis();
      screen[activeScreen]->activate(false);
      activeScreen = (activeScreen + 1) % NUMBEROFSCREENS;
      screen[activeScreen]->activate(true);
    }

    if (!digitalRead(INCREASEBUTTON)){
      lastPush = millis();
      screen[activeScreen]->modify(step);
    }

    if (!digitalRead(DECREASEBUTTON)){
      lastPush = millis();
      screen[activeScreen]->modify(-1 * step);
    }

    if (!(tick_counter % 5)) {
      eggTurnerTimer.check();
    }

    if (!(tick_counter % 25) && !measActive) {
      measActive = true;
      measType = TEMP;
      shtxx.meas(measType, &rawData, NONBLOCK);
    } 

    if (measActive && shtxx.measRdy()) {
      if (measType == TEMP) {
	temperature = shtxx.calcTemp(rawData);
	measType = HUMI;
	shtxx.meas(measType, &rawData, NONBLOCK);
      } else {
	humidity = shtxx.calcHumi(rawData, temperature);
	measActive = false;
	measReady = true;
      }
    }

    if (measReady) {
      measReady = false;
      temperatureSensor.update(temperature);
      humiditySensor.update(humidity);
      temperatureController.control();
      humidityController.control();
    }
    
    if (!(tick_counter % 50)){
      temperatureController.save(TEMPADDR);
      humidityController.save(HUMIDADDR);
      eggTurnerTimer.save(TURNERADDR);
    }
    
    tick_counter++;
    delay(200-(millis()-now));
  }

}
