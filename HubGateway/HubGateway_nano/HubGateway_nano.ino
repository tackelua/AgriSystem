#include <SoftwareSerial.h>
#include "GithChecksum.h"
#include "hardware.h"
#include <printf.h>
#include <ArduinoJson.h>
#include <nRF24L01.h>
#include <RF24.h>

String HUB_ID;
const char NODE_ID[] = "0543";

uint64_t RF_PIPE = 0xE8E8F0F0E1LL;
uint8_t RF_CHANNEL = 14;

RF24 radio(10, 9);

// //thay 10 thành 53 với mega
String radio_received;

SoftwareSerial CORE_SERIAL(CORE_SERIAL_TX, CORE_SERIAL_RX);

void setup() {
	delay(10);
	DEBUG.begin(115200);
	DEBUG.setTimeout(5);

	CORE_SERIAL.begin(9600);
	CORE_SERIAL.setTimeout(5);

#if defined(ARDUINO_AVR_MEGA2560)
	Serial.println(F("Hi I am Mega"));
#else
#if defined(ARDUINO_AVR_NANO)
	Serial.println(F("\r\n### N A N O ###"));
#endif
#endif

	hardware_init();

	radio_init();
	CORE_SERIAL.println(F("Skip first message"));
}

void loop() {
	//check_config_Serial();
	transfer_serial_radio();
	transfer_button_status();

}
