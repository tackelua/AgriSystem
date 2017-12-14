// 
// 
// 

#include "mqtt_helper.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "hardware.h"

const char* mqtt_server = "mic.duytan.edu.vn";
const char* mqtt_user = "Mic@DTU2017";
const char* mqtt_password = "Mic@DTU2017!@#";
const uint16_t mqtt_port = 1883;

extern String timeStr;

String mqtt_Message;
extern String HubID;
extern String MQTT_TOPIC_MAIN;

WiFiClient mqtt_espClient;
PubSubClient mqtt_client(mqtt_espClient);

void parseJsonFromServer(String& json) {
	StaticJsonBuffer<500> jsonBuffer;
	JsonObject& commands = jsonBuffer.parseObject(mqtt_Message);

	String jsMES_ID = commands["MES_ID"].as<String>();
	String jsHUB_ID = commands["HUB_ID"].as<String>();
	String jsSOURCE = commands["SOURCE"].as<String>();
	String jsDEST = commands["DEST"].as<String>();
	int jsCMD_T = commands["CMD_T"].as<int>();
	if (jsHUB_ID == HubID) {
		if (jsCMD_T == S2H_CONTROL_RELAY) {
			String jsLIGHT = commands["LIGHT"].as<String>();
			String jsFAN = commands["FAN"].as<String>();
			String jsMIST = commands["MIST"].as<String>();
			String jsCOVER = commands["COVER"].as<String>();

		}
	}
}

void mqtt_callback(char* topic, uint8_t* payload, unsigned int length) {
	ulong t = millis();
	//Dprint(F("\r\n#1 FREE RAM : "));
	//Dprintln(ESP.getFreeHeap());
	Dprintln(F("\r\n>>>"));

	String topicStr = topic;
	//Dprintln(topicStr);
	Dprint(F("Message arrived: "));
	Dprint(topicStr);
	Dprint(F("["));
	Dprint(length);
	Dprintln(F("]"));

	mqtt_Message = "";
	digitalWrite(LED_STATUS, LOW);
	for (uint i = 0; i < length; i++) {
		//Dprint((char)payload[i]);
		mqtt_Message += (char)payload[i];
	}
	mqtt_Message.trim();
	digitalWrite(LED_STATUS, HIGH);
	//Dprintln();
	//Dprint("String:");
	Dprintln(mqtt_Message);
	//Dprintln();

	//================================================

	if (topicStr == "DateTime")
	{
		//Dprintln(mqtt_Message);
		//Date: Mon, 19 Jun 2017 13:41:44 GMT
		timeStr = mqtt_Message.substring(23, 31);
		Dprintln(timeStr);

	}

	//control mist, light, fan
	else if (topicStr == MQTT_TOPIC_MAIN)
	{
		parseJsonFromServer(mqtt_Message);
	}

	Dprint(F("#2 FREE RAM : "));
	Dprintln(ESP.getFreeHeap());
	t = millis() - t;
	Dprintln("Time: " + String(t));
}

void mqtt_reconnect() {  // Loop until we're reconnected
	while (!mqtt_client.connected()) {
		Dprint(F("Attempting MQTT connection..."));
		//boolean connect(const char* id, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage);
		if (mqtt_client.connect(HubID.c_str(), mqtt_user, mqtt_password, MQTT_TOPIC_MAIN.c_str(), 0, true, String(HubID + " offline").c_str())) {
			Dprintln(F("connected"));
			mqtt_client.publish(MQTT_TOPIC_MAIN.c_str(), (HubID + " online").c_str(), true);
			mqtt_client.subscribe(MQTT_TOPIC_MAIN.c_str());
		}
		else {
			Dprint(F("failed, rc="));
			Dprint(mqtt_client.state());
			Dprintln(F(" try again"));
			delay(500);
		}
	}
}

void mqtt_init() {
	//http.setReuse(true);
	mqtt_Message.reserve(MQTT_MAX_PACKET_SIZE); //tao buffer khoang trong cho mqtt_Message
	mqtt_client.setServer(mqtt_server, mqtt_port);
	mqtt_client.setCallback(mqtt_callback);
}

void mqtt_loop() {
	if (!mqtt_client.connected()) {
		mqtt_reconnect();
	}
	mqtt_client.loop();
}

bool mqtt_publish(String topic, String payload, bool retain) {
	Dprint(F("MQTT publish to topic: "));
	Dprintln(topic);
	Dprintln(payload);
	Dprintln();

	digitalWrite(LED_STATUS, LOW);
	bool ret = mqtt_client.publish(topic.c_str(), payload.c_str(), retain);
	digitalWrite(LED_STATUS, HIGH);
	return ret;
}