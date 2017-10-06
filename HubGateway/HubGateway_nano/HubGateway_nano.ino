#include <SoftwareSerial.h>
#include "GithChecksum.h"
#include "hardware.h"
#include <printf.h>
#include <ArduinoJson.h>
#include <nRF24L01.h>
#include <RF24.h>

#define DB(x) Serial.println(x)
#define Db(x) Serial.print(x)


//#if defined(ARDUINO_AVR_MEGA2560)
//#define CSN 53
//#else
//#if defined(ARDUINO_AVR_NANO)
//#define CSN 10
//#endif
//#endif

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


SoftwareSerial ESP_Serial(ESP_SERIAL_RX, ESP_SERIAL_TX);
enum esp_command_code {
	REGISTER_TOPIC = 10,
	SEND_TO_SERVER
};

void setup() {
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
	DB(gcs_calc("abc"));
	H2N_infoResponse();
}

void loop() {
	//check_config_Serial();
	transfer_serial_radio();

	if (readButtons() != NO_BUTTON) {
		Serial.println(readButtons());
	}
}

void transfer_serial_radio() {
	if (Serial.available()) {
		radio_send(Serial.readString());
	}
	if (radio_available()) {
		Serial.println(radio_received);
	}
}