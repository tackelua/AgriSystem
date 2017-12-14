#include <WiFiClient.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

HardwareSerial Serial2(1);

#define _firmwareVersion = __DATE__ " " __TIME__;

#define DEBUG		Serial
#define Dprint		DEBUG.print
#define Dprintln	DEBUG.println
#define Dprintf		DEBUG.printf
#define Dflush		DEBUG.flush

#define RF			Serial2
#define Rprint		RF.print
#define Rprintln	RF.println
#define Rprintf		RF.printf
#define Rflush		RF.flush

#define LED_STATUS		LED_BUILTIN
#define HPIN_LIGHT		32
#define HPIN_FAN		33
#define HPIN_SPRAY		34
#define HPIN_COVER		-1 //virtual
#define HPIN_COVER_ON	35
#define HPIN_COVER_OFF	31

#define CMD_T		"CMD_T"
#define MES_ID		"MES_ID"
#define HUB_ID		"HUB_ID"
#define SOURCE		"SOURCE"
#define DEST		"DEST"
#define CMD_T		"CMD_T"
#define	SERVER		"SERVER"

#define ON			"ON"
#define OFF			"OFF"
#define MID			"MID"

#define LIGHT		"LIGHT"
#define FAN			"FAN"
#define SPRAY		"SPRAY"
#define COVER		"COVER"

#define MANURE		"MANURE"
#define LED_MOSFET	"LED_MOSFET"
#define TEMP		"TEMP"
#define HUMI		"HUMI"

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

enum NODE_TYPE {
	HUB_GATEWAY = 0,
	SOIL_MOISTURE,
	ENVIROMENT_STATION

};

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


void control_relay_hub(int HPIN, String STT, bool publish = false);
//=========================================================================

#pragma region WiFi Init
void wifi_init() {
	//WiFi.begin("IoT Wifi", "mic@dtu12345678()");

	WiFi.setAutoConnect(true);
	WiFi.setAutoReconnect(true);
	WiFi.mode(WIFI_STA);

	//Serial.println(F("SmartConfig started."));
	//WiFi.beginSmartConfig();
	//while (1) {
	//	delay(1000);
	//	if (WiFi.smartConfigDone()) {
	//		Serial.println(F("SmartConfig: Success"));
	//		WiFi.printDiag(Serial);
	//		//WiFi.stopSmartConfig();
	//		break;
	//	}
	//}

	WiFi.printDiag(Serial);
	Serial.println(F("\nConnecting..."));

	if (WiFi.waitForConnectResult() == WL_CONNECTED)
	{
		Serial.println(F("connected\n"));
	}
	else
	{
		Serial.println(F("connect again\n"));
		if (WiFi.waitForConnectResult() == WL_CONNECTED)
		{
			Serial.println(F("connected\n"));
			return;
		}

		Serial.println(F("SmartConfig started."));
		WiFi.beginSmartConfig();
		while (1) {
			delay(500);
			if (WiFi.smartConfigDone()) {
				Serial.println(F("SmartConfig: Success"));
				WiFi.printDiag(Serial);
				//WiFi.stopSmartConfig();
				break;
			}
		}
	}
}
#pragma endregion


#pragma region MQTT

const char* mqtt_server = "mic.duytan.edu.vn";
const char* mqtt_user = "Mic@DTU2017";
const char* mqtt_password = "Mic@DTU2017!@#";
const uint16_t mqtt_port = 1883;

WiFiClient mqtt_espClient;
PubSubClient mqtt_client(mqtt_espClient);

String timeStr;
String mqtt_Message(500);

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
			String jsSPRAY = commands["SPRAY"].as<String>();
			String jsCOVER = commands["COVER"].as<String>();

			control_relay_hub(HPIN_LIGHT, jsLIGHT);
			control_relay_hub(HPIN_FAN, jsFAN);
			control_relay_hub(HPIN_SPRAY, jsSPRAY);
			control_relay_hub(HPIN_COVER, jsCOVER);

			upload_relay_status();
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

	//control SPRAY, light, fan
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
#pragma endregion


#pragma region HARDWARE
#define STT_OFF			0
#define STT_ON			1
#define STT_COVER_MID	2

int STT_LIGHT = STT_OFF;
int STT_FAN = STT_OFF;
int STT_SPRAY = STT_OFF;
int STT_COVER = STT_OFF;

void hardware_init() {
	pinMode(HPIN_LIGHT, OUTPUT);
	pinMode(HPIN_FAN, OUTPUT);
	pinMode(HPIN_SPRAY, OUTPUT);
	pinMode(HPIN_COVER, OUTPUT);
}

void control_relay_hub(int HPIN, String STT, bool publish) {
	if (HPIN == HPIN_LIGHT) {
		if (STT == ON) {
			STT_LIGHT = STT_ON;
			digitalWrite(HPIN, true);
		}
		else if (STT == OFF) {
			STT_LIGHT = STT_OFF;
			digitalWrite(HPIN, true);
		}
	}
	else if (HPIN == HPIN_FAN) {
		if (STT == ON) {
			STT_FAN = STT_ON;
			digitalWrite(HPIN, true);
		}
		else if (STT == OFF) {
			STT_FAN = STT_OFF;
			digitalWrite(HPIN, true);
		}
	}
	else if (HPIN == HPIN_SPRAY) {
		if (STT == ON) {
			STT_SPRAY = STT_ON;
			digitalWrite(HPIN, true);
		}
		else if (STT == OFF) {
			STT_SPRAY = STT_OFF;
			digitalWrite(HPIN, true);
		}
	}
	else if (HPIN == HPIN_COVER) {
		if (STT == ON) {
			STT_COVER = STT_ON;
			digitalWrite(HPIN_COVER_ON, true);
			digitalWrite(HPIN_COVER_OFF, false);
		}
		else if (STT == OFF) {
			STT_COVER = STT_OFF;
			digitalWrite(HPIN_COVER_OFF, true);
			digitalWrite(HPIN_COVER_ON, false);
		}
		else if (STT == MID) {
			STT_COVER = STT_OFF;
			digitalWrite(HPIN_COVER_ON, false);
			digitalWrite(HPIN_COVER_OFF, false);
		}
	}
}

void upload_relay_status() {
	StaticJsonBuffer<500> jsBufferRelayHub;
	JsonObject& jsDataRelayHub = jsBufferRelayHub.createObject();
	jsDataRelayHub["MES_ID"] = MES_ID;
	jsDataRelayHub["HUB_ID"] = HubID;
	jsDataRelayHub["SOURCE"] = HubID;
	jsDataRelayHub["DEST"] = SERVER;
	jsDataRelayHub["CMD_T"] = H2S_UPDATE_HUB_STATUS;
	jsDataRelayHub["LIGHT"] = STT_LIGHT == STT_ON ? ON : OFF;
	jsDataRelayHub["FAN"] = STT_FAN == STT_ON ? ON : ON;
	jsDataRelayHub["SPRAY"] = STT_SPRAY == STT_ON ? ON : ON;
	jsDataRelayHub["COVER"] = STT_COVER == STT_ON ? ON : (STT_COVER == STT_OFF ? OFF : MID);

	String dataRelayHub;
	jsDataRelayHub.printTo(dataRelayHub);
	mqtt_publish(MQTT_TOPIC_MAIN, dataRelayHub, true);
}


#pragma endregion


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
