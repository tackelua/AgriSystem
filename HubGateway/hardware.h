// hardware.h

#ifndef _HARDWARE_h
#define _HARDWARE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#ifdef ARDUINO_AVR_NANO
	#define CONFIG_SERIAL	Serial
	#define CONFIG_SEN	A7

	#define CORE_SERIAL_TX	8
	#define CORE_SERIAL_RX	7

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

	extern bool relayStt_1;
	extern bool relayStt_2;
	extern bool relayStt_3;
	extern bool relayStt_4;

	#define RL_ON	HIGH
	#define RL_OFF	LOW

#endif // ARDUINO_AVR_NANO

#ifdef ARDUINO_ARCH_ESP8266
	#define LED_STATUS D3

	#define CORE_SERIAL_TX	D2
	#define CORE_SERIAL_RX	D1

	#define LCD_CLK	D0
	#define LCD_DIN	D5
	#define LCD_DC	D6
	#define LCD_CE	D7
	#define LCD_RST	D4
#endif // ARDUINO_ARCH_ESP8266


void hardware_init();

enum button_t {
	NO_BUTTON = 0,
	UP,
	DOWN,
	RIGHT,
	LEFT,
	BACK,
	ENTER
};

enum Node_t {
	HUB_GATEWAY = 0,
	SOIL_MOISTURE,
	ENVIROMENT_STATION

};

enum core_command_code {
	REGISTER_TOPIC = 10,
	SEND_TO_SERVER,
	SEND_BUTTON_PRESS
};
#endif

