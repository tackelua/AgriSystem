// hardware.h

#ifndef _HARDWARE_h
#define _HARDWARE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif
#include <SHT1x.h>

#define CONFIG_SERIAL	Serial
#define CONFIG_SEN	2

#define SHT_DATA	A4
#define SHT_CLK		A3

#define NRF_CSN		9
#define NRF_CE		10

#define LED_ON		LOW
#define LED_OFF		HIGH
#define LED_RELAY	3
#define LED_STATUS	4

#define RL_ON		HIGH
#define RL_OFF		LOW
#define RELAY		A5

void hardware_init();

enum Node_t {
	HUB_GATEWAY = 0,
	SOIL_MOISTURE,
	ENVIROMENT_STATION

};

#define DB(x) Serial.println(x)
#define Db(x) Serial.print(x)

extern SHT1x sensor;

#endif

