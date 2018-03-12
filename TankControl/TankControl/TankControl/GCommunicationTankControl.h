// GCommunicationTankControl.h

#ifndef _GCOMMUNICATIONTANKCONTROL_h
#define _GCOMMUNICATIONTANKCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif
#include <ArduinoJson.h>

#define RS485					Serial1
#define RS485_BAUDRATE			115200

enum COMMAND_TYPE {
	NO_COMMAND = 0, 

	CONTROL_GARDEN_HUB,
	CONTROL_GARDEN_NODE,
	CONTROL_ENVIROMENT_MONITOR,
	CONTROL_TANK_CONTROLER,

	GET_DATA_GARDEN_HUB,
	GET_DATA_GARDEN_NODE,
	GET_DATA_ENVIROMENT_MONITOR,
	GET_DATA_TANK_CONTROLER,

	UPDATE_DATA_GARDEN_HUB,
	UPDATE_DATA_GARDEN_NODE,
	UPDATE_DATA_ENVIROMENT_MONITOR,
	UPDATE_DATA_TANK_CONTROLER
};

enum NODE_TYPE {
	GARDEN_HUB = 0,
	GARDEN_NODE,
	ENVIROMENT_MONITOR,
	TANK_CONTROLER
};

/* #region CONTROL_TANK_CONTROLER
CONTROL_TANK_CONTROLER
Topic : "AGRISYSTEM/<HubID>"
{
	"MES_ID"	 : "<string>",
		"HUB_ID" : "<string>",
		"SOURCE" : "<string>",
		"DEST" : "<string>",
		"CMD_T" : CONTROL_TANK_CONTROLER,
		"WATER_IN" : "ON",
		"WATER_OUT" : "OFF"
}
GET_DATA_TANK_CONTROLER
Topic : "AGRISYSTEM/<HubID>"
{
	"MES_ID"	 : "<string>",
		"HUB_ID" : "<string>",
		"SOURCE" : "<string>",
		"DEST" : "<string>",
		"CMD_T" : GET_DATA_TANK_CONTROLER
}

UPDATE_DATA_TANK_CONTROLER
Topic : "AGRISYSTEM/<HubID>/<TankControlerID>"
{
	"MES_ID"	 : "<string>",
		"HUB_ID" : "<string>",
		"SOURCE" : "<string>",
		"DEST" : "<string>",
		"CMD_T" : UPDATE_DATA_TANK_CONTROLER,

		"WATER_IN" : "ON",
		"WATER_OUT" : "OFF",

		"WATER_LEVEL" : "<PERCENT>",
		"WATER_HIGH" : "YES",
		"WATER_LOW" : "NO"
}
#end region */

class GARDENCOMMUNICATION
{
public:
	bool getBool(String& cmd, String jkd);
	int getValue(String& cmd, String jkd);
	String getString(String& cmd, String jkd);
};

#endif

