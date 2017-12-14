// hardware.h

#ifndef _HARDWARE_h
#define _HARDWARE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


#define DEBUG		Serial
#define Dprint		DEBUG.print
#define Dprintln	DEBUG.println
#define Dprintf		DEBUG.printf
#define Dflush		DEBUG.flush

#define RF			Serial2
#define Rprint		RF.print
#define Rprintln	RF.println
#define Rprintf		RF.printf
#define Rflush		RF.flush

#define LED_STATUS BUILTIN_LED


void hardware_init();

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

enum NODE_TYPE {
	HUB_GATEWAY = 0,
	SOIL_MOISTURE,
	ENVIROMENT_STATION

};

class HUB_GATEWAYClass {

};
class SOIL_MOISTUREClass {

};
class ENVIROMENT_STATIONClass {

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

