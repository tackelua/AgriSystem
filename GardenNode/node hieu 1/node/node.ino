//MasterNode//
//Include 
#include <TimeLib.h>
#include <Time.h>
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
#define MES_ID		"MES_ID"
#define HUB_ID		"HUB_ID"
#define NODE_ID		"NODE_ID"
#define SOURCE		"SOURCE"
#define DEST		"DEST"
#define CMD_T		"CMD_T"
#define MANURE		"MANURE"
#define SPRAY		"SPRAY"
#define LIGHT		"LIGHT"
#define LED_MOSFET	"LED_MOSFET"

#define S_TEMP		"S_TEMP"
#define S_HUMI		"S_HUMI"
#define	S_LIGHT		"S_LIGHT"
#define	S_PH		"S_PH"
#define S_EC		"S_EC"

#define ON			"ON"
#define OFF			"OFF"
#define PC			"PC"

//STT
#define STT_OFF 0
#define	STT_ON 1
#define STT_PC 0

int Stt_Manure = STT_OFF;
int Stt_Spray = STT_OFF;
int Stt_Light = STT_OFF;
int Stt_LED_Mosfet = STT_PC;

//
//HardwareSerial
enum COMMAND_TYPE
{
	NO_COMMAND = 0, //pdhòng trường hợp ko có gì nó tự chuyển thành 0

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

enum NODE_TYPE
{
	GARDEN_HUB = 0,
	GARDEN_NODE,
	ENVIROMENT_MONITOR,
	TANK_CONTROLER
};

#endif // RF_Node

#define I2C_Sensor
#ifdef I2C_Sensor

// Define address sensor
#define SensorAddress 1 //address sensor

// Buffer data I2C sensor
float I2C_Buffer[6];

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

#endif // I2C_Sensor

#define HardWareNode
#ifdef HardWareNode
#define RESET_SENSOR	4
#define LED_RED			7
#define LED_BLUE		5
#define	RELAY1			21
#define	RELAY2			20
#define RELAY3			19
#define BUTTON			18
#define MOSFET			10

#define MANURE_PIN	RELAY1
#define SPRAY_PIN	RELAY2
#define LIGHT_PIN	RELAY3
#define LED_MOSFET_PIN MOSFET

#define LedRedOn	digitalWrite(LED_RED, LOW)
#define LedRedOff	digitalWrite(LED_RED, HIGH)
#define LedBlueOn	digitalWrite(LED_BLUE, LOW)
#define LedBlueOff	digitalWrite(LED_BLUE, HIGH)
#endif // HardwareNode

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
	//	if (RF.available() <= 0)
	//	{
	//		return 0;
	//	}
	String RF_Messenger;
	if (RF.available())
	{
		LedBlueOn;
		RF_Messenger.reserve(255);
		RF_Messenger = RF.readStringUntil('\n');
		DEBUG.print(F("Receive\r\n"));
		DEBUG.println(RF_Messenger);
		//DEBUG.print("\r\n");
		DynamicJsonBuffer jsonBufferNode(255);
		JsonObject& NodeBuffer = jsonBufferNode.parseObject(RF_Messenger);
		LedBlueOff;
		if (!NodeBuffer.success())
		{
			DEBUG.println(F("!NodeBuffer.success"));
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

				Upload_Data_Node();
			}
			if (_CmdT == GET_DATA_GARDEN_NODE)
			{
				Upload_Data_Node();
			}
		}
	}
}

void ControlOutput()
{
	(Stt_Manure == 1) ? digitalWrite(MANURE_PIN, HIGH) : digitalWrite(MANURE_PIN, LOW);
	(Stt_Spray == 1) ? digitalWrite(SPRAY_PIN, HIGH) : digitalWrite(SPRAY_PIN, LOW);
	(Stt_Light == 1) ? digitalWrite(LIGHT_PIN, HIGH) : digitalWrite(LIGHT_PIN, LOW);
	int ValM = map(Stt_LED_Mosfet, 0, 100, 0, 255);
	analogWrite(LED_MOSFET_PIN, ValM);
}
void Control_Output_Node(int PIN, String STT)
{

	if (PIN == MANURE_PIN)
	{
		if (STT == ON)
		{

			Stt_Manure = STT_ON;
			//digitalWrite(PIN, HIGH);
		}
		else if (STT == OFF)
		{
			Stt_Manure = STT_OFF;
			//digitalWrite(PIN, LOW);
		}
		(Stt_Manure == 1) ? digitalWrite(PIN, HIGH) : digitalWrite(PIN, LOW);
	}
	if (PIN == SPRAY_PIN)
	{
		if (STT == ON)
		{
			Stt_Spray = STT_ON;
			//digitalWrite(PIN, HIGH);
		}
		else if (STT == OFF)
		{
			Stt_Spray = STT_OFF;
			//digitalWrite(PIN, LOW);
		}
		(Stt_Spray == 1) ? digitalWrite(PIN, HIGH) : digitalWrite(PIN, LOW);
	}
	if (PIN == LIGHT_PIN)
	{
		if (STT == ON)
		{
			Stt_Light = STT_ON;
			digitalWrite(PIN, HIGH);
		}
		else if (STT == OFF)
		{
			Stt_Light = STT_OFF;
			digitalWrite(PIN, LOW);
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
		Upload_Data_Node();
	}
}

void Upload_Data_Node()
{
	LedBlueOn;
	DynamicJsonBuffer jsBufferUpDataNode(225);
	JsonObject& jsUpDataNode = jsBufferUpDataNode.createObject();

	jsUpDataNode[MES_ID] = String("HieuV1");
	jsUpDataNode[HUB_ID] = HubID;
	jsUpDataNode[SOURCE] = NodeID;
	jsUpDataNode[DEST] = HubID;
	jsUpDataNode[CMD_T] = int(UPDATE_DATA_GARDEN_NODE);

	jsUpDataNode[MANURE] = Stt_Manure == STT_ON ? ON : OFF;
	jsUpDataNode[SPRAY] = Stt_Spray == STT_ON ? ON : OFF;
	jsUpDataNode[LIGHT] = Stt_Light == STT_ON ? ON : OFF;
	{
		String pc = "PC";
		jsUpDataNode[LED_MOSFET] = pc + String(Stt_LED_Mosfet);
	}

	jsUpDataNode[S_TEMP] = String(I2C_Buffer[Temp]);
	jsUpDataNode[S_HUMI] = String(I2C_Buffer[Humi]);
	jsUpDataNode[S_EC] = String(I2C_Buffer[EC]);
	jsUpDataNode[S_LIGHT] = String(I2C_Buffer[Light]);
	jsUpDataNode[S_PH] = String(I2C_Buffer[PH]);

	String UpDataNode;
	jsUpDataNode.printTo(UpDataNode);
	if (RF.available() <= 0)
	{
		RF.println(UpDataNode);
		DEBUG.println(F("\r\nSend"));
		DEBUG.println(UpDataNode);
	}
	delay(10);
	LedBlueOff;
}

void I2C_ReadData()
{
	int Time_Read_Sensor = 10000;
	static unsigned long timeUD = millis();
	if ((millis() - timeUD) > Time_Read_Sensor)
	{
		timeUD = millis();
		I2C_Buffer[Temp] = callSensor(SensorAddress, Temp); // ok
		I2C_Buffer[Humi] = callSensor(SensorAddress, Humi); //ok
		I2C_Buffer[Light] = callSensor(SensorAddress, Light); //ok
		I2C_Buffer[PH] = callSensor(SensorAddress, PH); //ok
		I2C_Buffer[EC] = callSensor(SensorAddress, EC); //ok
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
		I2C_Buffer[status] = I2CData;
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
	DEBUG.begin(9600);
	//DEBUG.setTimeout(5);

	//RF setup
	delay(500);


	RF.begin(115200);
	RF.setTimeout(10);
	//Pin setup
	pinMode(RESET_SENSOR, OUTPUT);
	pinMode(LED_RED, OUTPUT);
	digitalWrite(LED_RED, HIGH);
	pinMode(LED_BLUE, OUTPUT);
	digitalWrite(LED_BLUE, HIGH);

	pinMode(MANURE_PIN, OUTPUT);
	pinMode(SPRAY_PIN, OUTPUT);
	pinMode(LIGHT_PIN, OUTPUT);

	pinMode(LED_MOSFET_PIN, OUTPUT);

	delay(10);
	//Stt_Manure = EEPROM.read(0x0E);
	//Stt_Spray = EEPROM.read(0x0F);
	//Stt_Light = EEPROM.read(0x10);
	//Stt_LED_Mosfet = EEPROM.read(0x11);

	//ControlOutput();

	//HubID = Read_Hub_ID();
	//NodeID = Create_Node_ID();
	NodeID = "NY2W81";
	HubID = "H4C37C";
	delay(100);
	I2C_Buffer[Temp] = callSensor(SensorAddress, Temp); // ok
	I2C_Buffer[Humi] = callSensor(SensorAddress, Humi); //ok
	I2C_Buffer[Light] = callSensor(SensorAddress, Light); //ok
	I2C_Buffer[PH] = callSensor(SensorAddress, PH); //ok
	I2C_Buffer[EC] = callSensor(SensorAddress, EC);
	delay(10);
	Upload_Data_Node();
	LedRedOn;
}


void loop()
{
	Control_Node();
	if (HubID != "")
	{
		I2C_ReadData();
		Update_data_Node(30 * 1000);
	}
}
