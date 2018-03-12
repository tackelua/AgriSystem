#include <EEPROM.h>
#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include "GCommunicationTankControl.h"
#include <SimpleKalmanFilter.h>
#include "SR04T.h"

#define DEBUG					Serial

#define LED_STATUS				16
#define LED_CONNECT				10
#define WATER_OUT_PIN			18
#define WATER_IN_PIN			19
#define SR04T_ECHOPIN			21// Pin to receive echo pulse
#define SR04T_TRIGPIN			9// Pin to send trigger pulse

#define EEPROM_ADDR_TANK_ID		0
#define EEPROM_ADDR_HUB_ID		10

GARDENCOMMUNICATION GC;
String sRS485_CMD;
String HUB_ID = "";
String TAN_ID = "T00000";

SimpleKalmanFilter kDSensor(5, 1, 2);
#define SR04T_SENSOR
#ifdef SR04T_SENSOR
SR04T sr04t_ss(SR04T_TRIGPIN, SR04T_ECHOPIN);
int iDistance = 0;
#endif // !SR04T_SENSOR

//#define IR03_SENSOR
#ifdef IR03_SENSOR
#define WD_INPIN		5
#define WD_OUTPIN		4
IR03 ir03_ss(WD_INPIN, WD_OUTPIN);
#endif // !IR03_SENSOR


int iWaterLevel = 0;
int iStatus;
bool bWaterIn, bWaterOut, bHighLevel, bLowLevel;

int counter = 0;
bool bLED_S, bLED_C;
byte gc_cmd;

String Tank_ID_Init()
{
	String ID;
	char flag_nid = EEPROM.read(EEPROM_ADDR_TANK_ID);//eeprom address flag is have id in eeprom
	//if (flag_nid != 'T')
	{
		EEPROM.write(0, 'T');	delay(5);
		for (uint8_t i = 1; i <= 5; i++)
		{
			char id;
			if (5 <= random(9))
			{
				id = random('0', '9');
			}
			else {
				id = random('A', 'Z');
			}
			EEPROM.write(i, id);
			delay(5);
		}
	}
	for (uint8_t i = EEPROM_ADDR_TANK_ID; i <= 5; i++)
	{
		ID += (char)EEPROM.read(i);
	}
	//DEBUG.print("node ID: ");
	//DEBUG.println(ID);
	return ID;
}
String Hub_ID_Init()
{
	String ID;
	char flag_nid = EEPROM.read(10);//eeprom address flag is have id in eeprom
	if (flag_nid == 'H') {
		ID = "H";
		for (uint8_t i = EEPROM_ADDR_HUB_ID + 1; i <= EEPROM_ADDR_HUB_ID + 8; i++)
		{
			char c = (char)EEPROM.read(i);
			if (c == 0) {
				return ID;
			}
			ID += (char)EEPROM.read(i);
		}
	}
	return ID;
}
void Save_Hub_ID(String ID)
{
	for (uint8_t i = EEPROM_ADDR_TANK_ID; i <= ID.length(); i++)
	{
		EEPROM.write(i, ID.charAt(i));
		delay(5);
	}
	EEPROM.write(EEPROM_ADDR_TANK_ID + ID.length() + 1, 0);
	delay(5);
	HUB_ID = ID;
}


void setup() {
	RS485.begin(RS485_BAUDRATE);
	RS485.setTimeout(100);
	DEBUG.begin(RS485_BAUDRATE);
	delay(1000);
	pinMode(LED_STATUS, OUTPUT);
	pinMode(LED_CONNECT, OUTPUT);
	digitalWrite(LED_CONNECT, HIGH);
	pinMode(WATER_OUT_PIN, OUTPUT);
	pinMode(WATER_IN_PIN, OUTPUT);
	
	DEBUG.println("Loading System Information...");

	TAN_ID = Tank_ID_Init();
	HUB_ID = Hub_ID_Init();

	DEBUG.print("HUB ID: "); DEBUG.println(HUB_ID);
	DEBUG.print("My ID: "); DEBUG.println(TAN_ID);

	DEBUG.println("Halo. I'm TankControl.");
}

// the loop function runs over and over again forever
void loop() {
	if (DEBUG.available())
	{
		sRS485_CMD = RS485.readString();
		DEBUG.print("RS485 --> ");
		DEBUG.println(sRS485_CMD);
		RS485.print(sRS485_CMD);
	}
	if (RS485.available())
	{
		digitalWrite(LED_CONNECT, LOW);
		delay(1);
		digitalWrite(LED_CONNECT, HIGH);
		sRS485_CMD = RS485.readString();
		DEBUG.print("RS485 <-- "); DEBUG.println(sRS485_CMD);
		RS485_Handle(sRS485_CMD);
	} 
	
	delay(1);
	if (counter++ > 50000) counter = 0;
	if ((counter % 200) == 0) // Test readSensor
	{
		bLED_S ^= 1;
#ifdef SR04T_SENSOR
		iDistance = kDSensor.updateEstimate(sr04t_ss.read());
#endif // !SR04T_SENSOR
	}
	if ((counter % 5000) == 0)
	{
		Update_TankData();
	}
}

void RS485_Handle(String& cmd)
{
	/* Check HUB_ID and DEST_ID */
	String h = GC.getString(cmd, "HUB_ID");
	String d = GC.getString(cmd, "DEST");

	if ((d = TAN_ID) && (h != HUB_ID)) {
		if (h.charAt(0) == 'H') {
			HUB_ID = h;
			Save_Hub_ID(HUB_ID);
		}
	}

	if (h != HUB_ID) return;
	if (d != TAN_ID) return;

	/* Get Command Type */
	int cmd_t = GC.getValue(cmd, "CMD_T");// DEBUG.println(a);

	switch (cmd_t)
	{
	case CONTROL_TANK_CONTROLER: //Control Tank Vavles
		bWaterIn = GC.getBool(cmd, "WATER_IN"); digitalWrite(WATER_IN_PIN, bWaterIn);
		bWaterOut = GC.getBool(cmd, "WATER_OUT"); digitalWrite(WATER_OUT_PIN, bWaterOut);
		Update_TankData();//Update Tank Data to Hub
		DEBUG.println("RS485 Control.");
		break;
	case GET_DATA_TANK_CONTROLER: //Request Tank Data
		Update_TankData();//Update Tank Data to Hub
		DEBUG.println("RS485 Request Data.");
		break;
	default: //Command Not Listed Yet
		break;
	}
}

void Update_TankData(void)
{
	if (RS485.available()) return;
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root["MES_ID"] = "LUAN";
	root["HUB_ID"] = HUB_ID;
	root["SOURCE"] = TAN_ID;
	root["DEST"] = HUB_ID;
	root["CMD_T"] = (int)UPDATE_DATA_TANK_CONTROLER;

	root["WATER_IN"] = bWaterIn ? "ON" : "OFF";
	root["WATER_OUT"] = bWaterOut ? "ON" : "OFF";

	root["WATER_LEVEL"] = (int)iDistance;
	root["WATER_HIGH"] = bHighLevel ? "ON" : "OFF";
	root["WATER_LOW"] = bLowLevel ? "ON" : "OFF";
	delay(random(10));
	if (RS485.available()) return;
	//root.printTo(DEBUG);
	root.printTo(RS485);
	//DEBUG.print(iDistance);
	//DEBUG.println("   cm");
}