//MasterNode//
//Include 
#include <TimeLib.h>
#include <Time.h>
#include <OneWire.h>
#include <SHT1x.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <Wire.h>

//Serial
#define DEBUG		Serial

#define flag_nID		0x06
#define flag_hID		0x0D
#define flag_ID_ON	5

#define RF_Node
#ifdef RF_Node

//Define ID
String HubID;
String NodeID;

#define RF			Serial1

//data json
#define MES_ID			"MES_ID"
#define HUB_ID			"HUB_ID"
#define NODE_ID			"NODE_ID"
#define SOURCE			"SOURCE"
#define DEST			"DEST"
#define CMD_T			"CMD_T"
#define MANURE			"MANURE"
#define ACTION_FROM		"ACTION_FROM"
#define SPRAY			"SPRAY"
#define LIGHT			"LIGHT"
#define LED_MOSFET		"LED_MOSFET"
#define LIGHT_MIN		"LIGHT_MIN"
#define LIGHT_MAX		"LIGHT_MAX" 
#define HUMI_MIN		"HUMI_MIN" 
#define HUMI_MAX		"HUMI_MAX" 
#define TEMP_MIN		"TEMP_MIN" 
#define TEMP_MAX		"TEMP_MAX" 
#define PH_MIN			"PH_MIN" 
#define PH_MAX			"PH_MAX" 
#define EC_MIN			"EC_MIN" 
#define EC_MAX			"EC_MAX" 
#define AUTO_STATUS		"AUTO_STATUS"
#define INTERVAL_UPDATE	"INTERVAL_UPDATE"
#define SCHELDULE		"SCHELDULE"

#define S_TEMP			"S_TEMP"
#define S_HUMI			"S_HUMI"
#define	S_LIGHT			"S_LIGHT"
#define	S_PH			"S_PH"
#define S_EC			"S_EC"

#define ON				"ON"
#define OFF				"OFF"
#define PC				"PC"

//STT
#define STT_OFF 0
#define	STT_ON 1
#define STT_PC 0

//
int	  BufferSTT[6];
float BufferSS[6];
float BufferCTl[6];
float BufferCTh[6];
int intervarUpdate;
String scheldule;
//
enum I2C_STT
{
	NoSensor = 0,
	Temp,
	Humi,
	Light,
	PH,
	EC
};

int Stt_Manure = STT_OFF;
int Stt_Spray = STT_OFF;
int Stt_Light = STT_OFF;
int Stt_LED_Mosfet = STT_PC;

//
//HardwareSerial
enum COMMAND_TYPE {
	NO_COMMAND = 0, //phòng trường hợp ko có gì nó tự chuyển thành 0

	CONTROL_GARDEN_HUB,							//no retain
	CONTROL_GARDEN_NODE,						//no retain
	CONTROL_ENVIROMENT_MONITOR,					//no retain
	CONTROL_TANK_CONTROLER,						//no retain

	GET_DATA_GARDEN_HUB,						//no retain
	GET_DATA_GARDEN_NODE,						//no retain
	GET_DATA_ENVIROMENT_MONITOR,				//no retain
	GET_DATA_TANK_CONTROLER,					//no retain

	UPDATE_DATA_GARDEN_HUB,						//with retain
	UPDATE_DATA_GARDEN_NODE,					//with retain
	UPDATE_DATA_ENVIROMENT_MONITOR,				//with retain
	UPDATE_DATA_TANK_CONTROLER,					//with retain

	UPDATE_HUB_HARDWARE_STATUS,					//with retain
	UPDATE_ACTION_LOGS,
	LIBS_GARDEN_NODE,

	NOTIFICATION,
	ADD_NEW_TRAY								//without retain
};

enum NODE_TYPE {
	GARDEN_HUB = 0,
	GARDEN_NODE,
	ENVIROMENT_MONITOR,
	TANK_CONTROLER
};

enum NOTIFICATION_TYPE {
	NO_NOTIFY = 0,

	HUB_ONLINE,
	HUB_OFFLINE,

	LIGHT_TOO_HIGH,
	LIGHT_TOO_LOW,

	TEMP_GARDEN_TOO_HIGH,
	TEMP_GARDEN_TOO_LOW,
	HUMI_GARDEN_TOO_LIGH,
	HUMI_GARDEN_TOO_LOW,

	TEMP_SOIL_TOO_LOW,
	TEMP_SOIL_TOO_HIGH,
	HUMI_SOIL_TOO_LOW,
	HUMI_SOIL_TOO_HIGH,

	RAIN,
	WINDY_TOO_STRONG,
	WATER_TOO_LOW,
	WATER_A_LOW,
	WATER_B_LOW,

	ERROR_SENSOR_TEMP,
	ERROR_SENSOR_HUMI,
	ERROR_SENSOR_LIGHT,
	ERROR_SENSOR_PH,
	ERROR_SENSOR_EC
};

#endif // RF_Node

#define I2C_Sensor
#ifdef I2C_Sensor

// Define address sensor
#define SensorAddress 1 //address sensor

#endif // I2C_Sensor

#define HardWareNode
#ifdef HardWareNode

#define SHTDataPin		20	
#define SHTClockPin		19
SHT1x SoilSS(SHTDataPin, SHTClockPin);

#define LightSSPin		9
#define PHSSPin			8
#define ECSSPin			6
#define TempECPin		5

#define LED_RED			18
#define LED_BLUE		7
#define BUTTON			21
#define ResetModlue		4

#define	RELAY1			16
#define	RELAY2			14
#define RELAY3			15
#define MOSFET			10

#define MANURE_PIN	RELAY1
#define SPRAY_PIN	RELAY2
#define LIGHT_PIN	RELAY3
#define LED_MOSFET_PIN MOSFET

#endif // HardwareNode
void Config_IO()
{
	pinMode(LightSSPin, INPUT);
	pinMode(PHSSPin, INPUT);
	pinMode(ECSSPin, INPUT);

	OneWire TempEC(TempECPin);

	pinMode(LED_RED, OUTPUT);
	digitalWrite(LED_RED, HIGH);

	pinMode(LED_BLUE, OUTPUT);
	digitalWrite(LED_BLUE, HIGH);

	pinMode(ResetModlue, OUTPUT);
	digitalWrite(ResetModlue, LOW);

	pinMode(MANURE_PIN, OUTPUT);
	digitalWrite(MANURE_PIN, LOW);

	pinMode(SPRAY_PIN, OUTPUT);
	digitalWrite(SPRAY_PIN, LOW);

	pinMode(LIGHT_PIN, OUTPUT);
	digitalWrite(LIGHT_PIN, LOW);

	pinMode(LED_MOSFET_PIN, OUTPUT);
	analogWrite(LED_MOSFET_PIN, 0);
}

//////////////////////////////////////////
String Create_Node_ID()
{
	char flag_nid;
	char id;
	String ID;
	flag_nid = EEPROM.read(flag_nID);
	if (flag_nid != flag_ID_ON)
	{
		EEPROM.write(0, 78);
		for (uint8_t i = 1; i <= 5; i++)
		{
			if (5 <= random(9))
			{
				id = random(48, 57);
			}
			else id = random(65, 90);
			Serial.println(id);
			EEPROM.write(i, id);
		}
		flag_nid = flag_ID_ON;
		EEPROM.write(flag_nID, flag_nid);
	}
	for (uint8_t i = 0; i <= 5; i++)
	{

		ID += (char)EEPROM.read(i);
	}
	DEBUG.print("node ID: ");
	DEBUG.println(ID);
	delay(1000);
	return ID;
}

/////////////////////////////////////////
void Write_Hub_ID(String ID)
{
	for (uint8_t i = 0; i <= 5; i++)
	{
		EEPROM.write(i + 0x07, ID.charAt(i));
	}
	EEPROM.write(flag_hID, flag_ID_ON);
}
String Read_Hub_ID()
{
	String ID = "";
	char flag_hid;
	flag_hid = EEPROM.read(flag_hID);
	if (flag_hid == flag_ID_ON)
	{
		for (uint8_t i = 0; i <= 5; i++)
		{
			ID += (char)EEPROM.read(i + 0x07);
		}
	}

	DEBUG.print("Hub ID: ");
	DEBUG.println(ID);
	delay(1000);
	return ID;
}


//////////////////////////////////////
int Control_Node()
{
	String RF_Messenger;
	if (RF.available())
	{
		RF_Messenger.reserve(255);
		RF_Messenger = RF.readStringUntil('\n');
		DEBUG.print(F("Receive\r\n"));
		DEBUG.println(RF_Messenger);
		DynamicJsonBuffer jsonBufferNode(255);
		JsonObject& NodeBuffer = jsonBufferNode.parseObject(RF_Messenger);
		if (!NodeBuffer.success())
		{
			DEBUG.println(F("Error! not successful"));
			return 0;
		}

		String _MesID = NodeBuffer[MES_ID].as<String>();
		String _HubID = NodeBuffer[HUB_ID].as<String>();
		String _Source = NodeBuffer[SOURCE].as<String>();
		String _Dest = NodeBuffer[DEST].as<String>();
		int _CmdT = NodeBuffer[CMD_T].as<int>();

		if (_Dest == NodeID)
		{
			if (_HubID != HubID)
			{
				HubID = _HubID;
				//Write_Hub_ID(HubID);
			}
			if (_CmdT == CONTROL_GARDEN_NODE)
			{
				String _Manure = NodeBuffer[MANURE].as<String>();
				/*{
				String _Manure = NodeBuffer[MANURE].as<String>();
				if (_Manure == ON)
				{
				Stt_Manure = 1;

				}
				else if (_Manure == OFF)
				{
				(Stt_Manure = 0);
				}
				//EEPROM.write(0x0E, Stt_Manure);
				}*/
				String _Spray = NodeBuffer[SPRAY].as<String>();
				/*{
				String _Spray = NodeBuffer[SPRAY].as<String>();
				if (_Spray == ON)
				{
				Stt_Spray = 1;
				}
				else if (_Spray == OFF)
				{
				(Stt_Spray = 0);
				}
				//EEPROM.write(0x0F, Stt_Spray);
				}*/
				String _Light = NodeBuffer[LIGHT].as<String>();
				/*{
				String _Light = NodeBuffer[LIGHT].as<String>();
				if (_Light == ON)
				{
				Stt_Light = 1;
				}
				else if (_Light == OFF)
				{
				(Stt_Light = 0);
				}
				//EEPROM.write(0x10, Stt_Light);
				}*/
				String _Led_Mosfet = NodeBuffer[LED_MOSFET].as<String>();
				/*{
				String _Led_Mosfet = NodeBuffer[LED_MOSFET].as<String>();
				if (_Led_Mosfet.indexOf("PC") >= 0)
				{
				String Stt_Tr = _Led_Mosfet.substring(2);
				int Val_Mosfet = Stt_Tr.toInt();
				Stt_LED_Mosfet = Val_Mosfet;
				}
				//EEPROM.write(0x11, Stt_LED_Mosfet);
				}*/
				//ControlOutput();
				Control_Output_Node(MANURE_PIN, _Manure);
				Control_Output_Node(SPRAY_PIN, _Spray);
				Control_Output_Node(LIGHT_PIN, _Light);
				Control_Output_Node(LED_MOSFET_PIN, _Led_Mosfet);

				Upload(1);
			}
			else if (_CmdT == GET_DATA_GARDEN_NODE)
			{
				Upload(0);
			}
			/*else if (_CmdT == LIBS_GARDEN_NODE)
			{
				BufferCTl[Light] = NodeBuffer[LIGHT_MIN].as<float>();
				BufferCTh[Light] = NodeBuffer[LIGHT_MAX].as<float>();
				BufferCTl[Humi] = NodeBuffer[HUMI_MIN].as<float>();
				BufferCTh[Humi] = NodeBuffer[HUMI_MAX].as<float>();
				BufferCTl[Temp] = NodeBuffer[TEMP_MIN].as<float>();
				BufferCTh[Temp] = NodeBuffer[TEMP_MAX].as<float>();
				BufferCTl[PH] = NodeBuffer[PH_MIN].as<float>();
				BufferCTh[PH] = NodeBuffer[PH_MAX].as<float>();
				BufferCTl[EC] = NodeBuffer[EC_MIN].as<float>();
				BufferCTh[EC] = NodeBuffer[EC_MAX].as<float>();

				intervarUpdate = NodeBuffer[INTERVAL_UPDATE].as<int>();
				scheldule = NodeBuffer[INTERVAL_UPDATE].as<String>();
			}*/
		}
	}
}

//void ControlOutput()
//{
//	(Stt_Manure == 1) ? digitalWrite(MANURE_PIN, HIGH) : digitalWrite(MANURE_PIN, LOW);
//	(Stt_Spray == 1) ? digitalWrite(SPRAY_PIN, HIGH) : digitalWrite(SPRAY_PIN, LOW);
//	(Stt_Light == 1) ? digitalWrite(LIGHT_PIN, HIGH) : digitalWrite(LIGHT_PIN, LOW);
//	int ValM = map(Stt_LED_Mosfet, 0, 100, 0, 255);
//	analogWrite(LED_MOSFET_PIN, ValM);
//}
void Control_Output_Node(int PIN, String STT)
{

	if (PIN == MANURE_PIN)
	{
		if (STT == ON)
		{
			Stt_Manure = STT_ON;
		}
		else if (STT == OFF)
		{
			Stt_Manure = STT_OFF;
		}

		(Stt_Manure == 1) ? digitalWrite(PIN, HIGH) : digitalWrite(PIN, LOW);
	}
	if (PIN == SPRAY_PIN)
	{
		if (STT == ON)
		{
			Stt_Spray = STT_ON;
		}
		else if (STT == OFF)
		{
			Stt_Spray = STT_OFF;
		}
		(Stt_Spray == 1) ? digitalWrite(PIN, HIGH) : digitalWrite(PIN, LOW);
	}
	if (PIN == LIGHT_PIN)
	{
		if (STT == ON)
		{
			Stt_Light = STT_ON;
		}
		else if (STT == OFF)
		{
			Stt_Light = STT_OFF;
		}
		(Stt_Light == 1) ? digitalWrite(PIN, HIGH) : digitalWrite(PIN, LOW);
	}
	if (PIN == LED_MOSFET_PIN)
	{
		if (STT.indexOf("PC") >= 0)
		{
			String Stt_Tr = STT.substring(2);
			int Val_Mosfet = Stt_Tr.toInt();
			Stt_LED_Mosfet = Val_Mosfet;
			int ValM = map(Val_Mosfet, 0, 100, 0, 255);
			analogWrite(PIN, ValM);
		}
	}
}

void Update_data_Node(unsigned int Time_Update_Data)
{
	static unsigned long timeUD = millis();
	if ((millis() - timeUD) > Time_Update_Data)
	{
		timeUD = millis();
		Upload(0);
	}
}

void Upload(bool i)
{
	DynamicJsonBuffer jsBufferUpDataNode(225);
	JsonObject& jsUpDataNode = jsBufferUpDataNode.createObject();

	jsUpDataNode[MES_ID] = String("HieuV2");
	jsUpDataNode[HUB_ID] = HubID;
	jsUpDataNode[SOURCE] = NodeID;
	jsUpDataNode[DEST] = HubID;
	jsUpDataNode[CMD_T] = int(UPDATE_DATA_GARDEN_NODE);
	if (i == 1)
	{
		jsUpDataNode[ACTION_FROM] = NodeID;
	}

	jsUpDataNode[MANURE] = Stt_Manure == STT_ON ? ON : OFF;
	jsUpDataNode[SPRAY] = Stt_Spray == STT_ON ? ON : OFF;
	jsUpDataNode[LIGHT] = Stt_Light == STT_ON ? ON : OFF;
	{
		String pc = "PC";
		jsUpDataNode[LED_MOSFET] = pc + String(Stt_LED_Mosfet);
	}

	jsUpDataNode[S_TEMP] = String(BufferSS[Temp]);
	jsUpDataNode[S_HUMI] = String(BufferSS[Humi]);
	jsUpDataNode[S_EC] = String(BufferSS[EC]);
	jsUpDataNode[S_LIGHT] = String(BufferSS[Light]);
	jsUpDataNode[S_PH] = String(BufferSS[PH]);

	String UpDataNode;
	jsUpDataNode.printTo(UpDataNode);
	if (RF.available() <= 0)
	{
		RF.println(UpDataNode);
		DEBUG.println(F("\r\nSend"));
		DEBUG.println(UpDataNode);
	}
	//delay(10);
}

void ReadSS()
{
	int Time_Read_Sensor = 5000;
	static unsigned long timeUD = millis();
	if ((millis() - timeUD) > Time_Read_Sensor)
	{
		timeUD = millis();
		BufferSS[Temp] = SoilSS.readTemperatureC();
		BufferSS[Humi] = SoilSS.readHumidity();
		BufferSS[Light] = analogRead(LIGHT_PIN);
		BufferSS[PH] = analogRead(PHSSPin);
		BufferSS[EC] = analogRead(ECSSPin);
		//if (!((BufferCTl[Temp] <= BufferSS[Temp]) && (BufferSS[Temp] <= BufferCTh[Temp])))
		//{
		//	if (!(BufferCTl[Temp] <= BufferSS[Temp])) //duoi gioi hang duoi
		//	{
		//		BufferSTT[Temp] = -1;
		//		//do something
		//		Control_Output_Node(LIGHT_PIN, ON);
		//	}
		//	else if (!(BufferSS[Temp] <= BufferCTh[Temp])) //tren gioi hang tren
		//	{
		//		BufferSTT[Temp] = 1;
		//		//do something
		//		Control_Output_Node(LIGHT_PIN, OFF);
		//	}
		//	else //nam trong gioi hang
		//	{
		//		BufferSTT[Temp] = 0;
		//		//do something
		//	}
		//	Upload();
		//}

		//if (!((BufferCTl[Humi] <= BufferSS[Humi]) && (BufferSS[Humi] <= BufferCTh[Humi])))
		//{
		//	if (!(BufferCTl[Humi] <= BufferSS[Humi])) //duoi gioi hang duoi
		//	{
		//		BufferSTT[Humi] = -1;
		//		//do something
		//		Control_Output_Node(MANURE_PIN, ON);
		//	}
		//	else if (!(BufferSS[Humi] <= BufferCTh[Humi])) //tren gioi hang tren
		//	{
		//		BufferSTT[Humi] = 1;
		//		//do something
		//		Control_Output_Node(MANURE_PIN, OFF);
		//	}
		//	else //nam trong gioi hang
		//	{
		//		BufferSTT[Humi] = 0;
		//		//do something
		//	}
		//	Upload();
		//}
		//if (!((BufferCTl[Light] <= BufferSS[Light]) && (BufferSS[Light] <= BufferCTh[Light])))
		//{
		//	if (!(BufferCTl[Light] <= BufferSS[Light])) //duoi gioi hang duoi
		//	{
		//		BufferSTT[Light] = -1;
		//		//do something
		//		Control_Output_Node(LIGHT_PIN, ON);
		//	}
		//	else if (!(BufferSS[Light] <= BufferCTh[Light])) //tren gioi hang tren
		//	{
		//		BufferSTT[Light] = 1;
		//		//do something
		//		Control_Output_Node(LIGHT_PIN, OFF);
		//	}
		//	else //nam trong gioi hang
		//	{
		//		BufferSTT[Light] = 0;
		//		//do something
		//	}
		//	Upload();
		//}
	}
}

void ReadI2CData()
{
	int Time_Read_Sensor = 3000;
	static unsigned long timeUD = millis();
	if ((millis() - timeUD) > Time_Read_Sensor)
	{
		timeUD = millis();
		BufferSS[Temp] = callSensor(SensorAddress, Temp); // ok
		BufferSS[Humi] = callSensor(SensorAddress, Humi); //ok
		BufferSS[Light] = callSensor(SensorAddress, Light); //ok
		BufferSS[PH] = callSensor(SensorAddress, PH); //ok
		BufferSS[EC] = callSensor(SensorAddress, EC); //ok
	}
}

float callSensor(int sensorID, int typeSensor)
{
	// Send Cmd
	float I2CData;
	int status = typeSensor;
	Wire.beginTransmission(sensorID);
	Wire.write(status >> 8);
	Wire.write(status & 0xff);
	Wire.endTransmission();

	// Rece data
	Wire.beginTransmission(sensorID);
	Wire.requestFrom(sensorID, 4);
	if (Wire.available())
	{
		byte data[4];
		int i = 0;
		while (Wire.available())
		{
			data[i] = Wire.read();
			i++;
		}
		union  By2Fl { byte ByinFl[4]; float Fl; } By2Fl;
		By2Fl.ByinFl[0] = data[0];
		By2Fl.ByinFl[1] = data[1];
		By2Fl.ByinFl[2] = data[2];
		By2Fl.ByinFl[3] = data[3];
		I2CData = By2Fl.Fl;
		BufferSS[status] = I2CData;
	}
	Wire.endTransmission();
	return I2CData;
}

void setup()
{
	delay(20);
	//i2c setup
	Wire.begin();

	//Serial
	DEBUG.begin(115200);
	//DEBUG.setTimeout(5);

	//RF setup
	delay(500);
	RF.begin(115200);
	RF.setTimeout(10);

	//Pin setup
	Config_IO();
	delay(10);

	//Stt_Manure = EEPROM.read(0x0E);
	//Stt_Spray = EEPROM.read(0x0F);
	//Stt_Light = EEPROM.read(0x10);
	//Stt_LED_Mosfet = EEPROM.read(0x11);
	//ControlOutput();
	//HubID = Read_Hub_ID();
	//NodeID = Create_Node_ID();

	NodeID = "N7G6F1";
	HubID = "H4C37C";
	ReadSS();
	Upload(0);
}


void loop()
{
	Control_Node();
	if (HubID != "")
	{
		ReadSS();
		Update_data_Node(30*1000);
	}

}