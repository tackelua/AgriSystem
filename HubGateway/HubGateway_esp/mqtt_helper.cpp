// 
// 
// 

#include "mqtt_helper.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "hardware.h"

extern String HubID;
extern String _firmwareVersion;
extern void updateFirmware(String url);
extern String CMD_ID;

const char* mqtt_server = "mic.duytan.edu.vn";
const char* mqtt_user = "Mic@DTU2017";
const char* mqtt_password = "Mic@DTU2017!@#";
const uint16_t mqtt_port = 1883;

const String on_ = "on";
const String off_ = "off";

int tempMax, tempMin, humiMax, humiMin, lightMax, lightMin;
bool library = false;

extern String timeStr;
extern bool stt_mist, stt_fan, stt_light;
extern void control(int pin, bool status, bool update_to_server = true);
extern void send_status_to_server();

String mqtt_Message;

WiFiClient mqtt_espClient;
PubSubClient mqtt_client(mqtt_espClient);

#pragma region parseTopic
void handleTopic__Mushroom_Commands_HubID() {
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& commands = jsonBuffer.parseObject(mqtt_Message);

	String HUB_ID = commands["HUB_ID"].as<String>();
	bool isResponse = (HUB_ID == "" ? true : false);

	static String last_CMD_ID = "xxxxxxxxxx"; //just reserve in ram
	CMD_ID = commands["CMD_ID"].as<String>();
	if ((CMD_ID != "") & (CMD_ID == last_CMD_ID)) {
		DEBUG.print(F("Command ID "));
		DEBUG.print(CMD_ID);
		DEBUG.print(F(" was excuted."));
		return;
	}
	DEBUG.println("last_CMD: " + last_CMD_ID);
	DEBUG.println("CMD_ID: " + CMD_ID);

	CMD_ID = String(millis());
	last_CMD_ID = CMD_ID;
	String mist_stt = commands["MIST"].as<String>();
	if (mist_stt == on_)
	{
		control(MIST, true, false);
	}
	else if (mist_stt == off_)
	{
		control(MIST, false, false);
	}

	String light_stt = commands["LIGHT"].as<String>();
	if (light_stt == on_)
	{
		control(LIGHT, true, false);
	}
	else if (light_stt == off_)
	{
		control(LIGHT, false, false);
	}

	String fan_stt = commands["FAN"].as<String>();
	if (fan_stt == on_)
	{
		control(FAN, true, false);
	}
	else if (fan_stt == off_)
	{
		control(FAN, false, false);
	}

	if (isResponse) {
		send_status_to_server();
	}
}

void handleTopic__Mushroom_Library_HubID() {
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& lib = jsonBuffer.parseObject(mqtt_Message);
	tempMax = lib["tempMax"].as<int>();
	tempMin = lib["tempMin"].as<int>();
	humiMax = lib["humiMax"].as<int>();
	humiMin = lib["humiMin"].as<int>();
	lightMax = lib["lightMax"].as<int>();
	lightMin = lib["lightMin"].as<int>();
	library = true;
}
#pragma endregion



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
	digitalWrite(LED, LOW);
	for (uint i = 0; i < length; i++) {
		//DEBUG.print((char)payload[i]);
		mqtt_Message += (char)payload[i];
	}
	digitalWrite(LED, HIGH);
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
	else if (topicStr == String("Mushroom/Commands/" + HubID))
	{
		handleTopic__Mushroom_Commands_HubID();
	}

	else if (topicStr == String("Mushroom/Library/" + HubID)) {
		handleTopic__Mushroom_Library_HubID();
	}

	else if (topicStr == "Mushroom/Terminal") {
		StaticJsonBuffer<250> jsonBuffer;
		JsonObject& terminal = jsonBuffer.parseObject(mqtt_Message);
		/*
		{
		   "Command" : "FOTA",
		   "HUB_ID" : "all",
		   "Version" : "",
		   "Url" : ""
		}
		*/
		DEBUG.println(F("Update firmware function"));
		String command = terminal["Command"].as<String>();
		if (command == "FOTA") {
			String hub = terminal["Hub_ID"].as<String>();
			if ((hub == HubID) || (hub == "all")) {
				String ver = terminal["Version"].as<String>();
				if (ver != _firmwareVersion) {
					String url = terminal["Url"].as<String>();
					mqtt_publish("Mushroom/Terminal/" + HubID, "Updating new firmware");
					DEBUG.print(F("\nUpdating new firmware: "));
					DEBUG.println(ver);
					DEBUG.println(url);
					updateFirmware(url);
					DEBUG.println(F("DONE!"));
				}
			}
		}
	}

	else if (topicStr == "Mushroom/Terminal/" + HubID) {
		if (mqtt_Message.indexOf("/get version") > -1) {
			//StaticJsonBuffer<200> jsBuffer;
			DynamicJsonBuffer jsBuffer(200);
			JsonObject& jsData = jsBuffer.createObject();
			jsData["HUB_ID"] = HubID;
			jsData["FW Version"] = _firmwareVersion;

			String data;
			data.reserve(100);
			jsData.printTo(data);
			mqtt_publish("Mushroom/Terminal/" + HubID, data);
		}

		String mqtt_cmd = mqtt_Message;
		mqtt_cmd.toUpperCase();
		if (mqtt_cmd.indexOf("LIGHT ON") > -1) {
			control(LIGHT, true, true);
		}
		if (mqtt_cmd.indexOf("LIGHT OFF") > -1) {
			control(LIGHT, false, true);
		}

		if (mqtt_cmd.indexOf("MIST ON") > -1) {
			control(MIST, true, true);
		}
		if (mqtt_cmd.indexOf("MIST OFF") > -1) {
			control(MIST, false, true);
		}

		if (mqtt_cmd.indexOf("FAN ON") > -1) {
			control(FAN, true, true);
		}
		if (mqtt_cmd.indexOf("FAN OFF") > -1) {
			control(FAN, false, true);
		}
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
		if (mqtt_client.connect(HubID.c_str(), mqtt_user, mqtt_password, ("Mushroom/Status/" + HubID).c_str(), 0, true, String(HubID + " offline").c_str())) {
			DEBUG.println(F("connected"));
			mqtt_client.publish(("Mushroom/Status/" + HubID).c_str(), (HubID + " online").c_str(), true);
			mqtt_client.publish(("Mushroom/SetWifi/" + HubID).c_str(), "Success");
			mqtt_client.subscribe("DateTime");
			mqtt_client.subscribe(("Mushroom/Library/" + HubID).c_str());
			mqtt_client.subscribe(("Mushroom/Commands/" + HubID).c_str());
			mqtt_client.subscribe("Mushroom/Terminal");
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

	digitalWrite(LED, LOW);
	bool ret = mqtt_client.publish(topic.c_str(), payload.c_str(), retain);
	digitalWrite(LED, HIGH);
	return ret;
}