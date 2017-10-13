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

#define DB(x) Serial.println(x)
#define Db(x) Serial.print(x)

extern SHT1x sensor;

//===========

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
	bool relay;
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

