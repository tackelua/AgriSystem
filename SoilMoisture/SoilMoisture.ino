
#include "hardware.h"
#include <printf.h>
#include <ArduinoJson.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SHT1x.h>

//#if defined(ARDUINO_AVR_MEGA2560)
//#define CSN 53
//#else
//#if defined(ARDUINO_AVR_NANO)
//#define CSN 10
//#endif
//#endif

String NODE_ID;

//RF24 radio(9, 10);
RF24 radio(NRF_CE, NRF_CSN);
String radio_received;


//====

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
//------
void setup()
{
	Serial.begin(115200);
	Serial.setTimeout(100);
#if defined(ARDUINO_AVR_MEGA2560)
	Serial.println(F("Hi I am Mega"));
#else
#if defined(ARDUINO_AVR_NANO)
	Serial.println(F("Hi I am Nano"));
#endif
#endif

	hardware_init();
	radio_init();
}

void loop()
{
	check_config_Serial();
	transfer_serial_radio();
	rf_command_handle();
	test_sensor();
}