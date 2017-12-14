#include <WiFiClient.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
//#include <SoftwareSerial.h>
#include "hardware.h"
#include "mqtt_helper.h"

HardwareSerial Serial2(1);

String _firmwareVersion = __DATE__ " " __TIME__;
String timeStr;

String getID() {
	byte mac[6];
	WiFi.macAddress(mac);
	String id;
	for (int i = 0; i < 16; i++)
	{
		id += String(mac[i], HEX);
	}
	id.toUpperCase();
	return id;
}
String HubID = getID();
String MQTT_TOPIC_MAIN = ("AGRISYSTEM/" + HubID);

void setup()
{
	delay(10);
	DEBUG.begin(115200);
	DEBUG.setTimeout(5);
	Dprintln(F("\r\n### E S P ###"));

	RF.begin(9600);
	RF.setTimeout(5);

	hardware_init();

	wifi_init();
}

void loop()
{
	delay(0);

}
