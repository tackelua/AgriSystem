#include <SoftwareSerial.h>
#include "GithChecksum.h"
#include "hardware.h"
#include <printf.h>
#include <ArduinoJson.h>
#include <nRF24L01.h>
#include <RF24.h>


#define DEBUG	Serial
#define DB(x)	DEBUG.println(x)
#define Db(x)	DEBUG.print(x)

String HUB_ID;

uint64_t RF_PIPE = 0xE8E8F0F0E1LL;
uint8_t RF_CHANNEL = 14;

RF24 radio(10, 9);

// //thay 10 thành 53 với mega
String radio_received;

enum COMMAND_TYPE {
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
	NODE_S1_TEMP_HUMI_RELAY,
	NODE_S2_LIGHT_RELAY
};
struct node_t {
	NODE_TYPE type;
	String id;
	float temp;
	float humi;
	String relay;
};

SoftwareSerial CORE_SERIAL(CORE_SERIAL_RX, CORE_SERIAL_TX);

void setup() {
	delay(10);
	DEBUG.begin(115200);
	DEBUG.setTimeout(10);

	CORE_SERIAL.begin(9600);
	CORE_SERIAL.setTimeout(20);

#if defined(ARDUINO_AVR_MEGA2560)
	Serial.println(F("Hi I am Mega"));
#else
#if defined(ARDUINO_AVR_NANO)
	Serial.println(F("\r\n### N A N O ###"));
#endif
#endif

	hardware_init();

	radio_init();
}

void loop() {
	//check_config_Serial();
	transfer_serial_radio();
	transfer_button_press();

}
