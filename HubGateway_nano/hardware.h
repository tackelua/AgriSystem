// hardware.h

#ifndef _HARDWARE_h
#define _HARDWARE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define CONFIG_SERIAL	Serial
#define CONFIG_SEN	A7

#define ESP_SERIAL_TX	8
#define ESP_SERIAL_RX	7

#define NRF_CSN		9
#define NRF_CE		10

#define BUZZER		6

#define RELAY_1		8
#define RELAY_2		7
#define RELAY_3		6
#define RELAY_4		5

#define SHT_SDA		A4
#define SHT_SCL		A6

#define INPUT_1		A3
#define INPUT_2		A2

#define BUTTONS		A1

#define LCD_LIGHT	A0

void hardware_init();

extern bool relayStt_1;
extern bool relayStt_2;
extern bool relayStt_3;
extern bool relayStt_4;

#define RL_ON	HIGH
#define RL_OFF	LOW
void pinOut(byte relay, bool status);

enum Node_t {
	HUB_GATEWAY = 0,
	SOIL_MOISTURE,
	ENVIROMENT_STATION

};

#endif

