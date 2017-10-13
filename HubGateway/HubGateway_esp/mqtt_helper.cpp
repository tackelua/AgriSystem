// 
// 
// 

#include "mqtt_helper.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
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
extern String mqtt_common_topic;

WiFiClient mqtt_espClient;
PubSubClient mqtt_client(mqtt_espClient);


void mqtt_callback(char* topic, uint8_t* payload, unsigned int length) {
	ulong t = millis();
	//DEBUG.print(F("\r\n#1 FREE RAM : "));
	//DEBUG.println(ESP.getFreeHeap());
	DEBUG.println(F("\r\n>>>"));

	String topicStr = topic;
	//DEBUG.println(topicStr);
	DEBUG.print(F("Message arrived: "));
	DEBUG.print(topicStr);
	DEBUG.print(F("["));
	DEBUG.print(length);
	DEBUG.println(F("]"));

	mqtt_Message = "";
	digitalWrite(LED_STATUS, LOW);
	for (uint i = 0; i < length; i++) {
		//DEBUG.print((char)payload[i]);
		mqtt_Message += (char)payload[i];
	}
	digitalWrite(LED_STATUS, HIGH);
	//DEBUG.println();
	//DEBUG.print("String:");
	DEBUG.println(mqtt_Message);
	//DEBUG.println();

	if (topicStr == "DateTime")
	{
		//DEBUG.println(mqtt_Message);
		//Date: Mon, 19 Jun 2017 13:41:44 GMT
		timeStr = mqtt_Message.substring(23, 31);
		DEBUG.println(timeStr);

#ifdef USE_OLED
		int sec, min, hour;
		hour = timeStr.substring(0, 2).toInt();
		min = timeStr.substring(3, 5).toInt();
		sec = timeStr.substring(6, 8).toInt();
		oled_analogClock(hour, min, sec, 0, 0);
#endif // USE_OLED

	}

	//control mist, light, fan
	else if (topicStr == "")
	{

	}

	DEBUG.print(F("#2 FREE RAM : "));
	DEBUG.println(ESP.getFreeHeap());
	t = millis() - t;
	DEBUG.println("Time: " + String(t));
}

void mqtt_reconnect() {  // Loop until we're reconnected
	while (!mqtt_client.connected()) {
		DEBUG.print(F("Attempting MQTT connection..."));
		//boolean connect(const char* id, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage);
		if (mqtt_client.connect(HubID.c_str(), mqtt_user, mqtt_password, mqtt_common_topic.c_str(), 0, true, String(HubID + " offline").c_str())) {
			DEBUG.println(F("connected"));
			mqtt_client.publish(mqtt_common_topic.c_str(), (HubID + " online").c_str(), true);
			mqtt_client.subscribe("DateTime");
			mqtt_client.subscribe(mqtt_common_topic.c_str());
		}
		else {
			DEBUG.print(F("failed, rc="));
			DEBUG.print(mqtt_client.state());
			DEBUG.println(F(" try again"));
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
	DEBUG.print(F("MQTT publish to topic: "));
	DEBUG.println(topic);
	DEBUG.println(payload);
	DEBUG.println();

	digitalWrite(LED_STATUS, LOW);
	bool ret = mqtt_client.publish(topic.c_str(), payload.c_str(), retain);
	digitalWrite(LED_STATUS, HIGH);
	return ret;
}