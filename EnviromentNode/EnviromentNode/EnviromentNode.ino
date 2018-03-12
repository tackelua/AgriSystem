/*
 Name:		EnviromentNode.ino
 Created:	1/4/2018 9:46:47 AM
 Author:	BaGac
*/
#include <ArduinoJson.h>
#include <SHT1x.h>
#include <Wire.h>
#include <TimerOne.h> 

#define RF			Serial1

#define MES_ID		"MES_ID"
#define HUB_ID		"HUB_ID"
#define NODE_ID		"NODE_ID"
#define SOURCE		"SOURCE"
#define DEST		"DEST"
#define CMD_T		"CMD_T"

#define S_RAIN		"S_RAIN"
#define S_WIND		"S_WIND"
#define	S_LIGHT		"S_LIGHT"
#define	S_TEMP		"S_TEMP"
#define S_HUMI		"S_HUMI"

#define WindSSPin A8
#define RainSSPin A9
#define LightSSPin A2
#define LEDSTT     A0

////// SHT Sensor /////////
#define SHTDataPin A7
#define SHTClock 7
SHT1x sht1x(SHTDataPin, SHTClock);
float temp_c;
float temp_f;
float humidity;
byte ArrayTempC[4];
byte ArrayHumidity[4];

typedef union {
	float flt;
	byte array[4];
} FloatConverter;

float ByteToFloat(byte *Array)
{
	int k_byte = 4;
	float value;
	FloatConverter aConverter;
	for (byte i = 0; i < k_byte; i++) {
		aConverter.array[i] = Array[i];
	}
	value = aConverter.flt;
	return value;
}
enum DefineSensorType
{
	NoSensor = 0,
	Temp,   // = 1
	Humi,   // = 2
	Light,  // = 3
	Rain,    // = 4
	Wind    // = 5
};

enum COMMAND_TYPE {
	NO_COMMAND = 0, //

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

float  I2C_Buffer[6];

/// SETTUPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP///

void setup()
{
	SensorInit();
	I2CInit();
	RFInit();
	Serial.begin(115200);
	delay(1000);
	Serial.println("START");
	//TimerProcess();
}
/// SETTUPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP///

void TimerProcess()
{
	Timer1.initialize(10000000); // khởi tạo timer 1 đến 1 giây
	Timer1.attachInterrupt(RFSenData); // khai báo ngắt timer 1
}
void SensorInit()
{
	pinMode(WindSSPin, INPUT);
	pinMode(RainSSPin, INPUT);
	pinMode(LightSSPin, INPUT);

	pinMode(LEDSTT, OUTPUT);
}
void I2CInit()
{
	Wire.begin();                // join i2c bus with address #8
}
void RFInit()
{
	RF.begin(115200);
	RF.setTimeout(10);
}

///// Temp and Humi Sensor /////////

void ReadDataSHT()
{
	// Read values from the sensor
	temp_c = sht1x.readTemperatureC();
	humidity = sht1x.readHumidity();
	delay(10);
}
////// Rain Sensor //////////////

int ReadRainSensor(int SSPin)
{
	int SS_array[20];
	int Count = 0;
	for (int i = 0; i <= 10; i++)
	{
		SS_array[i] = analogRead(SSPin);
		SS_array[20] = SS_array[20] + SS_array[i];
		Count++;
	}
	int Val = SS_array[20] / Count;
	if (Val >= 1000) return 0;
	else if (Val <= 100) return 1;
}

int ValShow = 0;
int ValNew = 0;
int ValCount = 0;
int ValRST = 0;
int TimeReadSHT = 0;
int Val = 0;
int ReceAndProcessData()
{
	ValNew = ReadRainSensor(RainSSPin);
	ValRST++;
	if (ValRST == 1500) 
	{ 
		TimeReadSHT++;
		ValCount = 0; 
		ValRST = 0; 
		return Val = 0;
	}
	if (ValNew != ValShow)
	{
		ValCount++;
		ValShow = ValNew;
		if (ValCount == 4) Val = 1;
	}
	//Serial.print("TimeReadSHT"); Serial.print(TimeReadSHT); Serial.print("\t");
	//Serial.print("ValRST"); Serial.print(ValRST); Serial.print("\t");
	//Serial.print("Val"); Serial.print(Val); Serial.print("\n");
	return Val;
}
int WindArray[10];
int MinWind = 50;
int MaxWind = 70;
void ReadI2CSensor(int TypeSS)
{
	switch (TypeSS)
	{
		case Rain:
		{
			//Serial.println("Rain    ");
			I2C_Buffer[TypeSS] = ReceAndProcessData();
			delay(10);
			break;
		}
		case Light:
		{
			//Serial.println("Light    ");
			int LightVal = 0;
			for (int i = 0; i < 5; i++)
			{
				LightVal = LightVal + analogRead(LightSSPin);
			}
			I2C_Buffer[TypeSS] = LightVal / 5;
			break;
		}
		case Temp:
		{
			//Serial.println("Temp    ");
			I2C_Buffer[TypeSS] = temp_c;
			break;
		}
		case Humi:
		{
			//Serial.println("Huim    ");
			I2C_Buffer[TypeSS] = humidity;
			break;
		}
		case Wind:
		{
			//Serial.println("Wind    ");
			int WindTemp = 0;
			int Count = 0;
			WindTemp = analogRead(WindSSPin);
			//Serial.print("WindTemp"); Serial.print(WindTemp); Serial.print(" \t");
			while (WindTemp < MinWind)
			{
				WindTemp = analogRead(WindSSPin);
				if (WindTemp > MaxWind )
				{
					Count = 0;
					break;
				}
				Count++;
			}
			//Serial.print("Count"); Serial.print(Count); Serial.print(" \n");
			//delay(100);
			if( Count > 0 && Count < 1200) I2C_Buffer[Wind] = Count;
			break;
		}

	}
}
String HubID = "H4C37C";
String NodeID = "EBG0111";

String EnviromentCreateString()
{
	DynamicJsonBuffer jsBufferUpDataNode(225);
	JsonObject& jsUpDataNode = jsBufferUpDataNode.createObject();

	jsUpDataNode[MES_ID] = String("BAGAC");
	jsUpDataNode[HUB_ID] = HubID;
	jsUpDataNode[SOURCE] = NodeID;
	jsUpDataNode[DEST] = HubID;
	jsUpDataNode[CMD_T] = int(UPDATE_DATA_ENVIROMENT_MONITOR);



	jsUpDataNode[S_TEMP] = String(I2C_Buffer[Temp]);
	jsUpDataNode[S_HUMI] = String(I2C_Buffer[Humi]);
	jsUpDataNode[S_LIGHT] = String(I2C_Buffer[Light]);
	//jsUpDataNode[S_TEMP] = random(25, 28);
	//jsUpDataNode[S_HUMI] = random(70,75);
	//jsUpDataNode[S_LIGHT] = random(2900, 3100);
	jsUpDataNode[S_RAIN] = String(I2C_Buffer[Rain]);
	jsUpDataNode[S_WIND] = String(I2C_Buffer[Wind]);

	String EnviromentNodeUpdate;
	jsUpDataNode.printTo(EnviromentNodeUpdate);
	return EnviromentNodeUpdate;
}
///// RF HC12 //////////
void RFSenData()
{
	String DATA = EnviromentCreateString();
	digitalWrite(LEDSTT, LOW);
	RF.println(DATA);
	delay(10);
	Serial.println("SEND");
	Serial.println(DATA);
	digitalWrite(LEDSTT, HIGH);
	//for (int i = 0; i < 3; i++)
	//{
	//	digitalWrite(LEDSTT, !(digitalRead(LEDSTT)));
	//	delay(1000);

	//}

}

/// LOPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP///


void loop()
{
	//if (TimeReadSHT == 3)
	//{
	//	TimeReadSHT = 0;
	//	ReadDataSHT();
	//	//Serial.print("TimeReadSHT: "); Serial.print(TimeReadSHT); Serial.print("\t");
	//	ReadI2CSensor(Temp); delay(10);
	//	ReadI2CSensor(Humi); delay(10);
	//	
	//}
	//else
	//{
	//	ReadI2CSensor(Wind);
	//	ReadI2CSensor(Rain);
	//	ReadI2CSensor(Light);
	//}

	readSHT();
	readWind();
	readRain();
	readLight();
	updateData(10000);
}

void updateData(unsigned long interval) {
	static unsigned long t = millis();
	if ((millis() - t) > interval) {
		t = millis();
		RFSenData();
	}
	delay(1);
}

void readSHT() {
	static unsigned long t = millis();
	if ((millis() - t) > 1000) {
		t = millis();
		ReadDataSHT();
		ReadI2CSensor(Temp); delay(10);
		ReadI2CSensor(Humi); delay(10);
	}
}

void readWind() {
	static unsigned long t = millis();
	if ((millis() - t) > 1000) {
		t = millis();
		ReadI2CSensor(Wind);
	}
}

void readRain() {
	static unsigned long t = millis();
	if ((millis() - t) > 1000) {
		t = millis();
		ReadI2CSensor(Rain);
	}
}

void readLight() {
	static unsigned long t = millis();
	if ((millis() - t) > 1000) {
		t = millis();
		ReadI2CSensor(Light);
	}
}

/// LOPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP///