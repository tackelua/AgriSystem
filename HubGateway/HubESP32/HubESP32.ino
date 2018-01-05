#include <esp_system.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define _firmwareVersion ("0.1.3" " " __DATE__ " " __TIME__)

HardwareSerial Serial2(2);

LiquidCrystal_I2C lcd(0x3f, 20, 4);

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
#define HPIN_LIGHT		34
#define HPIN_FAN		35
#define HPIN_SPRAY		32
#define HPIN_COVER		-1 //virtual
#define HPIN_COVER_ON	33
#define HPIN_COVER_OFF	25

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

enum COMMAND_TYPE {
	CONTROL_GARDEN_HUB = 0,
	CONTROL_GARDEN_NODE,
	CONTROL_ENVIROMENT_MONITOR,
	CONTROL_TANK_CONTROLER,

	GET_DATA_GARDEN_HUB,
	GET_DATA_GARDEN_NODE,
	GET_DATA_ENVIROMENT_MONITOR,
	GET_DATA_TANK_CONTROLER,

	UPDATE_DATA_GARDEN_HUB,
	UPDATE_DATA_GARDEN_NODE,
	UPDATE_DATA_ENVIROMENT_MONITOR,
	UPDATE_DATA_TANK_CONTROLER,
};
enum NODE_TYPE {
	GARDEN_HUB = 0,
	GARDEN_NODE,
	ENVIROMENT_MONITOR,
	TANK_CONTROLER
};


String getID() {
	uint8_t baseMac[6];
	// Get MAC address for WiFi station
	esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
	char baseMacChr[6] = { 0 };
	sprintf(baseMacChr, "%02X%02X%02X", baseMac[3], baseMac[4], baseMac[5]);
	String id = String(baseMacChr);
	id.trim();
	return id;
}
String HubID;
String MQTT_TOPIC_MAIN;


void control_relay_hub(int HPIN, String STT, bool publish = false);

//-------------------------------------------------------------------------
//LCD
#pragma region LCD

enum ALIGN {
	LEFT,
	MIDDLE,
	RIGHT
};
enum LINE {
	LINE1,
	LINE2,
	LINE3,
	LINE4
};
void lcd_print(String data, int line, int align = MIDDLE, int padding_left = 0) {
	if (line >= 4) { return; }
	String data_fullLine;
	int numSpace = 0;
	switch (align)
	{
	case LEFT:
		data_fullLine = data;
		numSpace = 20 - data.length();
		for (int i = 0; i < numSpace; i++)
		{
			data_fullLine += " ";
		}
		break;
	case MIDDLE:
		numSpace = (20 - data.length()) / 2;
		for (int i = 0; i < numSpace; i++)
		{
			data_fullLine += " ";
		}
		data_fullLine += data;
		for (int i = 0; i < numSpace; i++)
		{
			data_fullLine += " ";
		}
		break;
	case RIGHT:
		numSpace = 20 - data.length();
		for (int i = 0; i < numSpace; i++)
		{
			data_fullLine += " ";
		}
		data_fullLine += data;
		break;
	default:
		break;
	}
	for (int i = 0; i < padding_left; i++)
	{
		data_fullLine = " " + data_fullLine;
	}

	if (line < 4) {
		lcd.setCursor(0, line);
		lcd.print(data_fullLine);
	}
}

void lcd_init() {
	lcd.begin(21, 22);
	lcd.backlight();
	lcd_print("AGRISYSTEM/" + HubID, LINE1, MIDDLE);
	lcd_print("IoT Labs - MIC@DTU", LINE2, MIDDLE);
	lcd_print("Starting", LINE3, MIDDLE);
}

void lcd_showMainMenu() {
	lcd.clear();
	lcd_print("HUB STATUS", LINE1, LEFT, 1);
	lcd_print("NODE ID 1", LINE2, LEFT, 1);
	lcd_print("NODE ID 2", LINE3, LEFT, 1);

}
#pragma endregion

//=========================================================================

#pragma region WiFi Init
void wifi_init() {
	WiFi.begin("IoT Wifi", "mic@dtu12345678()"); return;

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

#define asdf BUILTIN_LED
const char* mqtt_server = "mic.duytan.edu.vn";
const char* mqtt_user = "Mic@DTU2017";
const char* mqtt_password = "Mic@DTU2017!@#";
const uint16_t mqtt_port = 1883;

WiFiClient mqtt_espClient;
PubSubClient mqtt_client(mqtt_espClient);

String timeStr;
String mqtt_Message;

String LibsNodes = "{}"; //String json chứa Libs của tất cả các Nodes
StaticJsonBuffer<1000> jsonBuffer;
JsonObject& LibsNodesJsObj = jsonBuffer.parseObject(LibsNodes);

void parseJsonMainFromServer(String& json) {
	StaticJsonBuffer<500> jsonBuffer;
	JsonObject& commands = jsonBuffer.parseObject(mqtt_Message);

	if (!commands.success()) {
		Dprintln(F("#ERR mqtt_Message invalid"));
		Dprintln();
		return;
	}

	String jsMES_ID = commands[MES_ID].as<String>();
	String jsHUB_ID = commands[HUB_ID].as<String>();
	String jsSOURCE = commands[SOURCE].as<String>();
	String jsDEST = commands[DEST].as<String>();
	int jsCMD_T = commands[CMD_T].as<int>();
	if (jsHUB_ID == HubID) {
		if (jsDEST == HubID) {
			if (jsCMD_T == CONTROL_GARDEN_NODE) {
				String jsLIGHT = commands[LIGHT].as<String>();
				String jsFAN = commands[FAN].as<String>();
				String jsSPRAY = commands[SPRAY].as<String>();
				String jsCOVER = commands[COVER].as<String>();

				control_relay_hub(HPIN_LIGHT, jsLIGHT);
				control_relay_hub(HPIN_FAN, jsFAN);
				control_relay_hub(HPIN_SPRAY, jsSPRAY);
				control_relay_hub(HPIN_COVER, jsCOVER);

				upload_relay_hub_status();
			}
		}
		else if (jsDEST != SERVER) {
			Dprintln(F("#SEND to RF: "));
			ulong t = millis();
			Dprintln(millis() - t);
			t = millis();
			Dprintln(mqtt_Message);
			Dflush();
			Dprintln(millis() - t);
			//Rprintln();
			Rprintln(mqtt_Message);
		}
	}
}

void parseJsonLibsFromServer(String& json) {
	//Phân tích json và lưu vào LibsNodes
	//Gửi json đến Node

	/* LibsNodes
	{
		"NODE1":{
			"TRAY_ID":"ABC",
			"HUB_CODE":"AB10027",
			"LIGHT_MIN":1,
			"LIGHT_MAX":1,
			"HUMI_MIN":1,
			"HUMI_MAX":1,
			"TEMP_MIN":1,
			"TEMP_MAX":1,
			"AUTO_STATUS":1,
			"INTERVAL_UPDATE":10
		},
		"NODE2":{
			"TRAY_ID":"ABC",
			"HUB_CODE":"AB10027",
			"LIGHT_MIN":1,
			"LIGHT_MAX":1,
			"HUMI_MIN":1,
			"HUMI_MAX":1,
			"TEMP_MIN":1,
			"TEMP_MAX":1,
			"AUTO_STATUS":1,
			"INTERVAL_UPDATE":10
		}
	}
	*/

	StaticJsonBuffer<500> jsonBufferServer;
	JsonObject& nodeLib = jsonBufferServer.parseObject(json);
	if (!nodeLib.success()) {
		Dprintln(F("#ERR json invalid"));
		Dprintln();
		return;
	}
	String TRAYID = nodeLib["TRAY_ID"].asString();
	String HUBCODE = nodeLib["HUB_CODE"].asString();
	int LIGHTMIN = nodeLib["LIGHT_MIN"].as<int>();
	int LIGHTMAX = nodeLib["LIGHT_MAX"].as<int>();
	int HUMIMIN = nodeLib["HUMI_MIN"].as<int>();
	int HUMIMAX = nodeLib["HUMI_MAX"].as<int>();
	int TEMPMIN = nodeLib["TEMP_MIN"].as<int>();
	int TEMPMAX = nodeLib["TEMP_MAX"].as<int>();
	int AUTOSTATUS = nodeLib["AUTO_STATUS"].as<int>();
	int INTERVALUPDATE = nodeLib["INTERVAL_UPDATE"].as<int>();

	JsonObject& TRAYDATA = LibsNodesJsObj.createNestedObject(TRAYID);
	TRAYDATA["TRAY_ID"] = TRAYID;
	TRAYDATA["HUB_CODE"] = HUBCODE;
	TRAYDATA["LIGHT_MIN"] = LIGHTMIN;
	TRAYDATA["LIGHT_MAX"] = LIGHTMAX;
	TRAYDATA["HUMI_MIN"] = HUMIMIN;
	TRAYDATA["HUMI_MAX"] = HUMIMAX;
	TRAYDATA["TEMP_MIN"] = TEMPMIN;
	TRAYDATA["TEMP_MAX"] = TEMPMAX;
	TRAYDATA["AUTO_STATUS"] = AUTOSTATUS;
	TRAYDATA["INTERVAL_UPDATE"] = INTERVALUPDATE;

	Dprintln(F("\r\nLibsNodes"));
	LibsNodes = "";
	LibsNodesJsObj.printTo(LibsNodes);
	LibsNodesJsObj.prettyPrintTo(DEBUG);
	Dprintln();

	Rprintln(json);
}

void mqtt_callback(char* topic, uint8_t* payload, unsigned int length) {
	ulong t = millis();
	//Dprint(F("\r\n#1 FREE RAM : "));
	//Dprintln(ESP.getFreeHeap());
	Dprintln(F("\r\nMQTT >>>"));

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
		parseJsonMainFromServer(mqtt_Message);
	}
	else if (topicStr.startsWith(MQTT_TOPIC_MAIN + "/LIBS/")) {
		parseJsonLibsFromServer(mqtt_Message);
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
		String h_offline = HubID + " offline";
		if (mqtt_client.connect(HubID.c_str(), mqtt_user, mqtt_password, MQTT_TOPIC_MAIN.c_str(), 0, true, h_offline.c_str())) {
			Dprintln(F("connected"));
			String h_online = HubID + " online";
			mqtt_client.publish(MQTT_TOPIC_MAIN.c_str(), h_online.c_str(), true);
			mqtt_client.subscribe(MQTT_TOPIC_MAIN.c_str());
			String libs = MQTT_TOPIC_MAIN + "/LIBS/#";
			mqtt_client.subscribe(libs.c_str());
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
		delay(1000);
		mqtt_reconnect();
	}
	mqtt_client.loop();
}

bool mqtt_publish(String topic, String payload, bool retain) {
	Dprint(F("\r\nMQTT publish to topic: "));
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
	delay(10);
	DEBUG.begin(115200);
	DEBUG.setTimeout(50);
	Dprintln(F("\r\n### E S P ###"));

	RF.begin(115200);
	RF.setTimeout(100);

	pinMode(HPIN_LIGHT, OUTPUT);
	pinMode(HPIN_FAN, OUTPUT);
	pinMode(HPIN_SPRAY, OUTPUT);
	pinMode(HPIN_COVER, OUTPUT);

	Dprintln();
	HubID = getID();
	MQTT_TOPIC_MAIN = "AGRISYSTEM/" + HubID;
	Dprintf("\r\n\r\nHID=%s\r\n\r\n", HubID.c_str());
	Dprintln(HubID);

	lcd_init();

	LibsNodes.reserve(1000);
}

void control_relay_hub(int HPIN, String STT, bool publish) {
	Dprintf("Turn pin %d %s\r\n", HPIN, STT == ON ? "on" : "off");

	if (HPIN == HPIN_LIGHT) {
		if (STT == ON) {
			STT_LIGHT = STT_ON;
			digitalWrite(HPIN, true);
		}
		else if (STT == OFF) {
			STT_LIGHT = STT_OFF;
			digitalWrite(HPIN, false);
		}
	}
	else if (HPIN == HPIN_FAN) {
		if (STT == ON) {
			STT_FAN = STT_ON;
			digitalWrite(HPIN, true);
		}
		else if (STT == OFF) {
			STT_FAN = STT_OFF;
			digitalWrite(HPIN, false);
		}
	}
	else if (HPIN == HPIN_SPRAY) {
		if (STT == ON) {
			STT_SPRAY = STT_ON;
			digitalWrite(HPIN, true);
		}
		else if (STT == OFF) {
			STT_SPRAY = STT_OFF;
			digitalWrite(HPIN, false);
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
			digitalWrite(HPIN_COVER_ON, false);
			digitalWrite(HPIN_COVER_OFF, true);
		}
		else if (STT == MID) {
			STT_COVER = STT_OFF;
			digitalWrite(HPIN_COVER_ON, false);
			digitalWrite(HPIN_COVER_OFF, false);
		}
	}
}

void upload_relay_hub_status() {
	DynamicJsonBuffer jsBufferRelayHub(500);
	JsonObject& jsDataRelayHub = jsBufferRelayHub.createObject();

	jsDataRelayHub[MES_ID] = String(micros());
	jsDataRelayHub[HUB_ID] = HubID;
	jsDataRelayHub[SOURCE] = HubID;
	jsDataRelayHub[DEST] = SERVER;
	jsDataRelayHub[CMD_T] = int(UPDATE_DATA_GARDEN_HUB);
	jsDataRelayHub[LIGHT] = STT_LIGHT == STT_ON ? ON : OFF;
	jsDataRelayHub[FAN] = STT_FAN == STT_ON ? ON : ON;
	jsDataRelayHub[SPRAY] = STT_SPRAY == STT_ON ? ON : ON;
	jsDataRelayHub[COVER] = STT_COVER == STT_ON ? ON : (STT_COVER == STT_OFF ? OFF : MID);

	String dataRelayHub;
	jsDataRelayHub.printTo(dataRelayHub);
	mqtt_publish(MQTT_TOPIC_MAIN, dataRelayHub, true);
}

void handle_rf_communicate() {
	if (RF.available() <= 0) {
		return;
	}
	String rf_Message;

	rf_Message = RF.readStringUntil('\n');
	rf_Message.trim();
	if (rf_Message.length() == 0) {
		return;
	}
	Dprintln(F("\r\nRF >>> "));
	Dprintln(rf_Message);
	//Rprint(F("RF>> "));
	//Rprintln(rf_Message);
	DynamicJsonBuffer jsonBufferNodeData(500);
	JsonObject& nodeData = jsonBufferNodeData.parseObject(rf_Message);

	if (!nodeData.success()) {
		Dprintln(F("#ERR rf_Message invalid"));
		//Dprintln(rf_Message);
		Dprintln();
		return;
	}

	String _hubID = nodeData[HUB_ID].as<String>();
	String _DEST = nodeData[DEST].as<String>();
	String _SOURCE = nodeData[SOURCE].as<String>();
	if (_hubID == HubID) {
		String nodeDataString;
		if (_DEST == HubID) {
			nodeData[DEST] = SERVER;
			nodeData.printTo(nodeDataString);
		}
		else {
			nodeDataString = rf_Message;
		}
		mqtt_publish(MQTT_TOPIC_MAIN + "/" + _SOURCE, nodeDataString, true);
	}
}

void handle_serial() {
	if (Serial.available()) {
		String s = Serial.readString();
		Dprintln(F("Serial >>> "));
		Dprintln(s);
		Rprintln(s);
	}
}
#pragma endregion


void setup()
{
	hardware_init();

	wifi_init();
	WiFi.waitForConnectResult();
	if (WiFi.localIP() == IPAddress(0, 0, 0, 0)) {
		ESP.restart();
	}
	Dprint(F("Local IP: "));
	Dprintln(WiFi.localIP());

	lcd_print("WiFi connected", LINE3, MIDDLE);
	lcd_print(WiFi.localIP().toString(), LINE4, MIDDLE);

	mqtt_init();

	lcd_showMainMenu();
}

void loop()
{
	mqtt_loop();
	handle_rf_communicate();
	handle_serial();
	delay(0);
}
