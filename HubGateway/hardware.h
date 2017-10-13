// hardware.h

#ifndef _HARDWARE_h
#define _HARDWARE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define DEBUG	Serial
#define DB(x)	DEBUG.println(x)
#define Db(x)	DEBUG.print(x)
//#define db		DEBUG.printf

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
	CC_SEND_TO_SERVER = 0,
	CC_SEND_BUTTON_PRESS
};

enum COMMAND_TYPE
{
	S2H_CONTROL_RELAY = 0,
	S2H_GET_HUB_STATUS,
	S2H_GET_SENSOR_DATA,

	H2S_UPDATE_HUB_STATUS = 10,
	H2S_UPDATE_NODE_DATA,

	N2H_REGISTER_NEW_NODE,
	N2H_DATA_FROM_SENSORS,
	H2N_INFO_RESPONSE,			//
	H2N_GET_DATA				//
};

enum NODE_TYPE
{
	NODE_S1_TEMP_HUMI_RELAY,
	NODE_S2_LIGHT_RELAY
};
struct node_t
{
	NODE_TYPE type;
	String id;
	float temp;
	float humi;
	String relay;
};

#define Githkey		"Gith"
#define CMD_T		"CMD_T"
#define NODE_T		"NODE_T"
#define NID			"NID"
#define TEMP		"TEMP"
#define HUMI		"HUMI"
#define RL_STT		"RL_STT"
#define GCS			"GCS"
#define RF_ADDR		"RF_ADDR"
#define RF_CHN		"RF_CHN"
#define ON			"ON"
#define OFF			"OFF"
#define HID			"HID"
#define R1			"R1"
#define R2			"R2"
#define R3			"R3"
#define R4			"R4"
#define DATA		"DATA"
#endif

