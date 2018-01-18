#pragma region DECLARATION

#include <TimeLib.h>
#include <Time.h>
#include <esp_system.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define _FIRMWARE_VERSION ("0.1.10" " " __DATE__ " " __TIME__)

HardwareSerial Serial2(2);

LiquidCrystal_I2C lcd(0x3f, 20, 4);

#define DB_BAUDRATE	921600
#define RF_BAUDRATE	115200

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

#define LED_STATUS			LED_BUILTIN
#define HPIN_LIGHT			33
#define HPIN_FAN			25
#define HPIN_SPRAY			26
#define HPIN_COVER			-1 //virtual
#define HPIN_COVER_OPEN		27
#define HPIN_COVER_CLOSE	14
//--
#define HPIN_BUTTON			34
enum BUTTON {
	BT_NOBUTTON,
	BT_LEFT,
	BT_DOWN,
	BT_RIGHT,
	BT_UP,
	BT_BACK,	//yellow
	BT_ENTER	//red
};


#define CMD_T		"CMD_T"
#define MES_ID		"MES_ID"
#define HUB_ID		"HUB_ID"
#define SOURCE		"SOURCE"
#define DEST		"DEST"
#define TIMESTAMP	"TIMESTAMP"
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
	NO_COMMAND = 0, //phòng trường hợp ko có gì nó tự chuyển thành 0

	CONTROL_GARDEN_HUB,
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

	UPDATE_DATA_HUB_HARDWARE_STATUS,
	LIBS_GARDEN_NODE
};
enum NODE_TYPE {
	GARDEN_HUB = 0,
	GARDEN_NODE,
	ENVIROMENT_MONITOR,
	TANK_CONTROLER
};

String HubID;
String MQTT_TOPIC_MAIN;

String mqtt_Message;
String rf_Message;
#pragma endregion


#pragma region HARDWARE
#define STT_OFF			0
#define STT_ON			1
#define STT_COVER_MID	2

int STT_LIGHT = STT_OFF;
int STT_FAN = STT_OFF;
int STT_SPRAY = STT_OFF;
int STT_COVER = STT_OFF;

String getID() {
	return "H4C37C";
	uint8_t baseMac[6];
	// Get MAC address for WiFi station
	esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
	char baseMacChr[6] = { 0 };
	sprintf(baseMacChr, "%02X%02X%02X", baseMac[3], baseMac[4], baseMac[5]);
	String id = String(baseMacChr);
	id = "H" + id.substring(1);
	id.trim();
	return id;
}

void hardware_init() {
	delay(10);
	DEBUG.begin(DB_BAUDRATE);
	DEBUG.setTimeout(50);
	Dprintln(F("\r\n### E S P ###"));

	RF.begin(RF_BAUDRATE);
	RF.setTimeout(200);

	pinMode(HPIN_LIGHT, OUTPUT);
	pinMode(HPIN_FAN, OUTPUT);
	pinMode(HPIN_SPRAY, OUTPUT);
	pinMode(HPIN_COVER_OPEN, OUTPUT);
	pinMode(HPIN_COVER_CLOSE, OUTPUT);
	pinMode(HPIN_BUTTON, INPUT);
	//Dprintln("LIGHT ON");  digitalWrite(HPIN_LIGHT, HIGH);							delay(500);
	//Dprintln("LIGHT OFF"); digitalWrite(HPIN_LIGHT, LOW);							delay(500);
	//																				delay(500);
	//Dprintln("FAN ON");  digitalWrite(HPIN_FAN, HIGH);								delay(500);
	//Dprintln("FAN OFF"); digitalWrite(HPIN_FAN, LOW);								delay(500);
	//																				delay(500);
	//Dprintln("SPRAY ON");  digitalWrite(HPIN_SPRAY, HIGH);							delay(500);
	//Dprintln("SPRAY OFF"); digitalWrite(HPIN_SPRAY, LOW);							delay(500);
	//																				delay(500);
	//Dprintln("COVER_OPEN ON");  digitalWrite(HPIN_COVER_OPEN, HIGH);				delay(500);
	//Dprintln("COVER_OPEN OFF"); digitalWrite(HPIN_COVER_OPEN, LOW);					delay(500);
	//																				delay(500);
	//Dprintln("COVER_CLOSE ON");  digitalWrite(HPIN_COVER_CLOSE, HIGH);				delay(500);
	//Dprintln("COVER_CLOSE OFF"); digitalWrite(HPIN_COVER_CLOSE, LOW);				delay(500);
	//Dprintln("LIGHT ON");  digitalWrite(HPIN_LIGHT, HIGH);							delay(500);
	//Dprintln("LIGHT OFF"); digitalWrite(HPIN_LIGHT, LOW);							delay(500);
	//																				delay(500);
	//Dprintln("FAN ON");  digitalWrite(HPIN_FAN, HIGH);								delay(500);
	//Dprintln("FAN OFF"); digitalWrite(HPIN_FAN, LOW);								delay(500);
	//																				delay(500);
	//Dprintln("SPRAY ON");  digitalWrite(HPIN_SPRAY, HIGH);							delay(500);
	//Dprintln("SPRAY OFF"); digitalWrite(HPIN_SPRAY, LOW);							delay(500);
	//																				delay(500);
	//Dprintln("COVER_OPEN ON");  digitalWrite(HPIN_COVER_OPEN, HIGH);				delay(500);
	//Dprintln("COVER_OPEN OFF"); digitalWrite(HPIN_COVER_OPEN, LOW);					delay(500);
	//																				delay(500);
	//Dprintln("COVER_CLOSE ON");  digitalWrite(HPIN_COVER_CLOSE, HIGH);				delay(500);
	//Dprintln("COVER_CLOSE OFF"); digitalWrite(HPIN_COVER_CLOSE, LOW);				delay(500);

	Dprintln();
	HubID = getID();
	MQTT_TOPIC_MAIN = "AGRISYSTEM/" + HubID;
	Dprintf("\r\n\r\nHID=%s\r\n\r\n", HubID.c_str());
	Dprintln(HubID);
	Dprintln("Version: " + String(_FIRMWARE_VERSION));

	lcd_init();

}

void control_relay_hub(int HPIN, String STT, bool publish = false) {
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
			digitalWrite(HPIN_COVER_OPEN, true);
			digitalWrite(HPIN_COVER_CLOSE, false);
		}
		else if (STT == OFF) {
			STT_COVER = STT_OFF;
			digitalWrite(HPIN_COVER_OPEN, false);
			digitalWrite(HPIN_COVER_CLOSE, true);
		}
		else if (STT == MID) {
			STT_COVER = STT_OFF;
			digitalWrite(HPIN_COVER_OPEN, false);
			digitalWrite(HPIN_COVER_CLOSE, false);
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
	jsDataRelayHub[TIMESTAMP] = String(now());
	jsDataRelayHub[CMD_T] = int(UPDATE_DATA_GARDEN_HUB);
	jsDataRelayHub[LIGHT] = STT_LIGHT == STT_ON ? ON : OFF;
	jsDataRelayHub[FAN] = STT_FAN == STT_ON ? ON : OFF;
	jsDataRelayHub[SPRAY] = STT_SPRAY == STT_ON ? ON : OFF;
	jsDataRelayHub[COVER] = STT_COVER == STT_ON ? ON : (STT_COVER == STT_OFF ? OFF : MID);

	String dataRelayHub;
	jsDataRelayHub.printTo(dataRelayHub);
	mqtt_publish(MQTT_TOPIC_MAIN, dataRelayHub, true);
}

void handle_rf_communicate() {
	if (RF.available() <= 0) {
		return;
	}
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
		static byte total_rf_fail = 0;
		if (++total_rf_fail > 5) {
			total_rf_fail = 0;
			Dprintln(F("RESET RF SERIAL"));
			//RF.end();
			//delay(1);
			//RF.begin(RF_BAUDRATE);
			//RF.flush();
			ESP.deepSleep(100000);
		}
		Dprintln();
		return;
	}

	String _hubID = nodeData[HUB_ID].as<String>();
	String _DEST = nodeData[DEST].as<String>();
	String _SOURCE = nodeData[SOURCE].as<String>();
	nodeData[TIMESTAMP] = String(now());
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

int map_value_to_button(int val) {
	//	BT_LEFT    : 520	 || 452	 - 588
	//	BT_DOWN	   : 656	 || 588	 - 760
	//	BT_RIGHT   : 864	 || 760	 - 1042
	//	BT_UP	   : 1220	 || 1042 - 1570
	//	BT_BACK	   : 1920	 || 1570 - 2657
	//	BT_ENTER   : 4095	 || 2657 - 4095
	if ((430 < val) && (val <= 588)) {
		//Dprintln(F("BT_LEFT"));
		return BT_LEFT;
	}
	if ((588 < val) && (val <= 760)) {
		//Dprintln(F("BT_DOWN"));
		return BT_DOWN;
	}
	if ((760 < val) && (val <= 1042)) {
		//Dprintln(F("BT_RIGHT"));
		return BT_RIGHT;
	}
	if ((1042 < val) && (val <= 1570)) {
		//Dprintln(F("BT_UP"));
		return BT_UP;
	}
	if ((1570 < val) && (val <= 2657)) {
		//Dprintln(F("BT_BACK"));
		return BT_BACK;
	}
	if ((2657 < val) && (val <= 4095)) {
		//Dprintln(F("BT_ENTER"));
		return BT_ENTER;
	}
	return BT_NOBUTTON;
}
int button_read() {
	static const int total = 20;
	static int pre[total];
	static unsigned long t = millis();
	static const unsigned long debound_time = 3;
	static int last_button = BT_NOBUTTON;
	if ((millis() - t) > debound_time) {
		t = millis();
		for (int i = total - 1; i > 0; i--) {
			pre[i] = pre[i - 1];
		}
		pre[0] = map_value_to_button(analogRead(HPIN_BUTTON));
		bool stable = true;
		for (int i = 1; i < total; i++) {
			if (pre[i] != pre[0]) {
				stable = false;
			}
		}
		if (stable && (last_button != pre[0])) {
			if (last_button == BT_NOBUTTON) {
				switch (pre[0])
				{
				case BT_LEFT:
					Dprintln("BT_LEFT");
					break;
				case BT_DOWN:
					Dprintln("BT_DOWN");
					break;
				case BT_RIGHT:
					Dprintln("BT_RIGHT");
					break;
				case BT_UP:
					Dprintln("BT_UP");
					break;
				case BT_BACK:
					Dprintln("BT_BACK");
					break;
				case BT_ENTER:
					Dprintln("BT_ENTER");
					break;
				default:
					break;
				}
				for (int i = 0; i < total; i++) {
					Dprint(pre[i]);
					Dprint(" ");
				}
				Dprintln();
			}
			last_button = pre[0];
			return pre[0];
		}
	}
	return BT_NOBUTTON;
}
#pragma endregion


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
	Dprintln(F("\r\nConnecting to WiFi")); 
	if (WiFi.isConnected()) {
		return;
	}
	WiFi.begin("IoT Wifi", "mic@dtu12345678()");
	WiFi.waitForConnectResult();
	while (1) {
		if (WiFi.isConnected()) {
			break;
		}
		delay(1);
	}
	Dprint(F("Local IP: "));
	Dprintln(WiFi.localIP());

	return;

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

StaticJsonBuffer<10000> jsonBuffer;
JsonObject& ListGardenDevicesJs = jsonBuffer.parseObject("{}");
JsonArray& ListGardenDevicesJsArray = ListGardenDevicesJs.createNestedArray("List");

void parseJsonMainFromServer(String& json) {
	StaticJsonBuffer<500> jsonBuffer;
	JsonObject& commands = jsonBuffer.parseObject(json);

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
			if (jsCMD_T == CONTROL_GARDEN_HUB) {
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
			else if (jsCMD_T == GET_DATA_GARDEN_HUB) {
				upload_relay_hub_status();
			}
		}
		else if (jsDEST != SERVER) {
			Rprintln(json);
			Dprintln(F("#SEND to RF: "));
			//ulong t = millis();
			//Dprintln(millis() - t);
			//t = millis();
			Dprintln(json);
			Dflush();
			delay(10); //!important
		}
	}
}

bool check_NodeIsExist_in_ListGardenDevicesJs(String trayID) {
	/*
	{
	   "List":[
		  "Tray ID 1",
		  "Tray ID 2",
		  "Tray ID 3",
		  "Tray ID 4",
		  "Tray ID 5"
	   ]
	}
	*/
}

void parseJsonLibsFromServer(String& json) {
	//Phân tích json và lưu vào LibsNodes
	//Gửi json đến Node

	/* ListGardenDevicesJs
	{
		"MES_ID"		: "<string>",
		"HUB_ID"		: "<string>",
		"SOURCE"		: "<string>",
		"DEST"		: "<string>",
		"TIMESTAMP"	: "<long>",
		"CMD_T"		: LIBS_GARDEN_NODE,

		"TRAY_NAME"	: "Cải", //bỏ dấu please
		"LIGHT_MIN"	: 1,
		"LIGHT_MAX"	: 1,
		"HUMI_MIN"	: 1,
		"HUMI_MAX"	: 1,
		"TEMP_MIN"	: 1,
		"TEMP_MAX"	: 1,
		"AUTO_STATUS"	: 1,
		"INTERVAL_UPDATE": int(second),
		"SCHELDULE"	: "TimeStampStart_TimeStampStop, ..." //Giờ bật_tắt
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
	String TRAYNAME = nodeLib["TRAY_NAME"].asString();
	int LIGHTMIN = nodeLib["LIGHT_MIN"].as<int>();
	int LIGHTMAX = nodeLib["LIGHT_MAX"].as<int>();
	int HUMIMIN = nodeLib["HUMI_MIN"].as<int>();
	int HUMIMAX = nodeLib["HUMI_MAX"].as<int>();
	int TEMPMIN = nodeLib["TEMP_MIN"].as<int>();
	int TEMPMAX = nodeLib["TEMP_MAX"].as<int>();
	int AUTOSTATUS = nodeLib["AUTO_STATUS"].as<int>();
	int INTERVALUPDATE = nodeLib["INTERVAL_UPDATE"].as<int>();

	JsonObject& TRAYDATA = ListGardenDevicesJs.createNestedObject(TRAYID);
	ListGardenDevicesJsArray.add("Tray ID 1");

	Dprintln(F("\r\nLibsNodes"));
	ListGardenDevicesJs.prettyPrintTo(DEBUG);
	Dprintln();

	Rprintln(json);
}

void mqtt_callback(char* topic, uint8_t* payload, unsigned int length) {
	//ulong t = millis();

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

	//control SPRAY, light, fan
	if (topicStr == MQTT_TOPIC_MAIN)
	{
		parseJsonMainFromServer(mqtt_Message);
	}
	else if (topicStr.startsWith(MQTT_TOPIC_MAIN + "/LIBS/")) {
		parseJsonLibsFromServer(mqtt_Message);
	}

	//Dprint(F("#2 FREE RAM : "));
	//Dprintln(ESP.getFreeHeap());
	//t = millis() - t;
	//Dprintln("Time: " + String(t));
}

void mqtt_reconnect() {  // Loop until we're reconnected
	while (!mqtt_client.connected()) {
		Dprint(F("Attempting MQTT connection..."));
		//boolean connect(const char* id, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage);
		String h_offline = "{\"HUB_ID\":\"" + HubID + "\",\"STATUS\":\"OFFLINE\"}";
		String topic_HUBSTATUS = MQTT_TOPIC_MAIN + "/STATUS";
		if (mqtt_client.connect(HubID.c_str(), mqtt_user, mqtt_password, topic_HUBSTATUS.c_str(), 0, true, h_offline.c_str())) {
			Dprintln(F("connected"));
			String h_online = "{\"HUB_ID\":\"" + HubID + "\",\"STATUS\":\"ONLINE\"}";
			mqtt_client.publish(topic_HUBSTATUS.c_str(), h_online.c_str(), true);
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
	rf_Message.reserve(MQTT_MAX_PACKET_SIZE);
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


#pragma region HTTP
String http_request(String host, uint16_t port = 80, String url = "/") {
	Dprintln("\r\nGET " + host + ":" + String(port) + url);
	WiFiClient client;
	if (!client.connect(host.c_str(), port)) {
		Dprintln("connection failed");
		return "";
	}
	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
		"Host: " + host + "\r\n" +
		"Connection: close\r\n\r\n");
	unsigned long timeout = millis();
	while (client.available() == 0) {
		if (millis() - timeout > 2000) {
			Dprintln(">>> Client Timeout !");
			client.stop();
			return "";
		}
	}

	// Read all the lines of the reply from server and print them to Serial
	//while (client.available()) {
	//	String line = client.readStringUntil('\r');
	//	Dprint(line);
	//}
	//Dprintln();
	//Dprintln();
	String body;
	if (client.available()) {
		body = client.readString();
		int pos_body_begin = body.indexOf("\r\n\r\n");
		if (pos_body_begin > 0) {
			body = body.substring(pos_body_begin + 4);
		}
	}
	client.stop();
	body.trim();
	return body;
}
#pragma endregion

#pragma region TASKS
void updateTimeStamp(unsigned long interval = 0) {
	static unsigned long t_pre_update = 0;
	static bool isSync = false;
	if (interval == 0) {
		String strTimeStamp = http_request("mic.duytan.edu.vn", 88, "/api/GetUnixTime");
		Dprintln(strTimeStamp);
		DynamicJsonBuffer timestamp(500);
		JsonObject& jsTimeStamp = timestamp.parseObject(strTimeStamp);
		if (jsTimeStamp.success()) {
			time_t ts = String(jsTimeStamp["UNIX_TIME"].asString()).toInt();
			if (ts > 1000000000) {
				t_pre_update = millis();
				isSync = true;
				setTime(ts);
				Dprintln(F("Time Updated\r\n"));
				return;
			}
		}
	}
	else {
		if ((millis() - t_pre_update) > interval) {
			updateTimeStamp();
		}
	}
	if (!isSync) {
		updateTimeStamp();
	}
}

int wifi_quality(int32_t dBm) {
	WiFi.RSSI();
	if (dBm <= -100)
		return 0;
	else if (dBm >= -50)
		return 100;
	else
		return int(2 * (dBm + 100));
}
void updateHubHardwareStatus(unsigned long interval = 5000) {
	static unsigned long t = millis();
	if ((millis() - t) > interval) {
		t = millis();
		DynamicJsonBuffer HubHardwareStatus(500);
		JsonObject& jsHubHardwareStatus = HubHardwareStatus.createObject();
		jsHubHardwareStatus[MES_ID] = String(millis());
		jsHubHardwareStatus[HUB_ID] = HubID;
		jsHubHardwareStatus[SOURCE] = HubID;
		jsHubHardwareStatus[DEST] = SERVER;
		jsHubHardwareStatus[TIMESTAMP] = String(now());
		jsHubHardwareStatus[CMD_T] = UPDATE_DATA_HUB_HARDWARE_STATUS;
		jsHubHardwareStatus["WIFI_SIGNAL"] = String(wifi_quality(WiFi.RSSI()));
		jsHubHardwareStatus["TEMP_INTERNAL"] = String(temperatureRead(), 2);

		String strHubHardwareStatus;
		jsHubHardwareStatus.printTo(strHubHardwareStatus);

		String dataRelayHub;
		mqtt_publish(MQTT_TOPIC_MAIN, strHubHardwareStatus, true);
	}
}
#pragma endregion



void setup()
{
	hardware_init();

	wifi_init();

	lcd_print("WiFi connected", LINE3, MIDDLE);
	lcd_print(WiFi.localIP().toString(), LINE4, MIDDLE);

	updateTimeStamp();
	mqtt_init();

	lcd_showMainMenu();
}

void loop()
{
	updateHubHardwareStatus(5000);
	updateTimeStamp(60000);
	mqtt_loop();
	handle_rf_communicate();
	handle_serial();
	button_read();
	delay(0);
}