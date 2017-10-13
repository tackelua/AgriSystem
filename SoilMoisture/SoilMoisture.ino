
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

String HubID;
String NodeID;

//RF24 radio(9, 10);
RF24 radio(NRF_CE, NRF_CSN);
String radio_received;


//====
node_t thisNode;
void node_init() {
	thisNode.type = NODE_S1_TEMP_HUMI_RELAY;
	thisNode.id = NodeID;
}
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