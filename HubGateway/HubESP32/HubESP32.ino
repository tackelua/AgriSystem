#pragma region DECLARATION

#include <QList.h>
#include <TimeLib.h>
#include <Time.h>
#include <esp_system.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define _FIRMWARE_VERSION ("0.1.22 doing" " " __DATE__ " " __TIME__)

HardwareSerial Serial2(2);

LiquidCrystal_I2C lcd(0x3f, 20, 4);
#define SB_SELECT	0 //symbol select
byte SELECT[8] =
{
	B10000,
	B11000,
	B11100,
	B11110,
	B11100,
	B11000,
	B10000,
	B00000
};

String HubID;
String MQTT_TOPIC_MAIN;		//"AGRISYSTEM/" + HubID;

String mqtt_Message;
String rf_Message;


const char* mqtt_server = "mic.duytan.edu.vn";
const char* mqtt_user = "Mic@DTU2017";
const char* mqtt_password = "Mic@DTU2017!@#";
const uint16_t mqtt_port = 1883;

WiFiClient mqtt_espClient;
PubSubClient mqtt_client(mqtt_espClient);


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

#define IDLE	delayMicroseconds(200);

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

enum ALIGN {
	LEFT,
	MIDDLE,
	RIGHT
};
enum LINE {
	LINE0,
	LINE1,
	LINE2,
	LINE3
};

#define REQUEST			"REQUEST"
#define RESPONSE		"RESPONSE"
#define CMD_T			"CMD_T"
#define MES_ID			"MES_ID"
#define HUB_ID			"HUB_ID"
#define HUB_NAME		"HUB_NAME"
#define SOURCE			"SOURCE"
#define DEST			"DEST"
#define TIMESTAMP		"TIMESTAMP"
#define CMD_T			"CMD_T"
#define	SERVER			"SERVER"

#define	HUB_CODE		"HUB_CODE"
#define	ACTION_NAME		"ACTION_NAME"
#define	ACTION_FROM		"ACTION_FROM"
#define	ACTION_TO		"ACTION_TO"
#define APP				"APP"

#define WIFI_SIGNAL		"WIFI_SIGNAL"
#define TEMP_INTERNAL	"TEMP_INTERNAL"

#define ON				"ON"
#define OFF				"OFF"
#define MID				"MID"

#define LIGHT			"LIGHT"
#define FAN				"FAN"
#define SPRAY			"SPRAY"
#define COVER			"COVER"

#define MANURE			"MANURE"
#define LED_MOSFET		"LED_MOSFET"
#define TEMP			"TEMP"
#define HUMI			"HUMI"
#define S_TEMP			"S_TEMP"
#define S_HUMI			"S_HUMI"
#define S_LIGHT			"S_LIGHT"	
#define S_PH			"S_PH"
#define S_EC			"S_EC"	


enum COMMAND_TYPE {
	NO_COMMAND = 0, //phòng trường hợp ko có gì nó tự chuyển thành 0

	CONTROL_GARDEN_HUB,							//no retain
	CONTROL_GARDEN_NODE,						//no retain
	CONTROL_ENVIROMENT_MONITOR,					//no retain
	CONTROL_TANK_CONTROLER,						//no retain

	GET_DATA_GARDEN_HUB,						//no retain
	GET_DATA_GARDEN_NODE,						//no retain
	GET_DATA_ENVIROMENT_MONITOR,				//no retain
	GET_DATA_TANK_CONTROLER,					//no retain

	UPDATE_DATA_GARDEN_HUB,						//with retain
	UPDATE_DATA_GARDEN_NODE,					//with retain
	UPDATE_DATA_ENVIROMENT_MONITOR,				//with retain
	UPDATE_DATA_TANK_CONTROLER,					//with retain

	UPDATE_HUB_HARDWARE_STATUS,					//with retain
	UPDATE_ACTION_LOGS,
	LIBS_GARDEN_NODE,

	NOTIFICATION,
	ADD_NEW_TRAY								//without retain
};

enum DEVICE_TYPE {
	UNKNOWN = 0,
	GARDEN_HUB,
	GARDEN_NODE,
	ENVIROMENT_MONITOR,
	TANK_CONTROLER
};

class Devices_Info {
public:
	String ID = "";
	String Name = "";
	DEVICE_TYPE Type = UNKNOWN;
	Devices_Info(String id = "", String name = "") {
		//!important
		if ((id == "") && (name == "")) {
			return;
		}
		ID = id;
		Name = name;
		switch (id.charAt(0))
		{
		case 'H':
			Type = GARDEN_HUB;
			break;
		case 'N':
			Type = GARDEN_NODE;
			break;
		case 'E':
			Type = ENVIROMENT_MONITOR;
			break;
		case 'T':
			Type = TANK_CONTROLER;
			break;
		default:
			break;
		}
		printDetails();
	};
	void printDetails(Print& source = DEBUG) {
		Dprintln("Devices_Info");
		Dprintf("\tID\t=\t%s\r\n", ID.c_str());
		Dprintf("\tName\t=\t%s\r\n", Name.c_str());
		Dprintf("\tType\t=\t%d\r\n", (int)Type);
		Dprintln();
	}
};
inline bool operator==(const Devices_Info& d1, const Devices_Info& d2) {
	return ((d1.Name == d2.Name) && (d1.ID == d2.ID));
}

QList<Devices_Info> DevicesList; //index 0 is Hub
//int DevicesList_length() {
//	return DevicesList.length() - 1;
//}
//===============
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

String getTimeString() {
	return (hour() < 10 ? "0" + String(hour()) : String(hour())) + ":" + (minute() < 10 ? "0" + String(minute()) : String(minute())) + ":" + (second() < 10 ? "0" + String(second()) : String(second()));
}

void hardware_init() {
	delay(10);
	DEBUG.begin(DB_BAUDRATE);
	DEBUG.setTimeout(50);
	Dprintln(F("\r\n### E S P ###"));

	//RF.begin(RF_BAUDRATE);
	//RF.setTimeout(200);

	pinMode(LED_BUILTIN, OUTPUT);
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
	Dprintln();
	WiFi.printDiag(DEBUG);
	Dprintln();
}


#pragma region LCD

enum LCD_Pages {
	LCD_PAGE_MAIN_MENU = 0,
	LCD_PAGE_DETAIL_HUB,
	LCD_PAGE_DETAIL_NODE,
	LCD_PAGE_DETAIL_ENVI,
	LCD_PAGE_DETAIL_TANK
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
		if ((data_fullLine.length() + padding_left) > 20) {
			data_fullLine = data_fullLine.substring(0, 20);
		}
		lcd.setCursor(0, line);
		lcd.print(data_fullLine);
	}
}

void lcd_init() {
	lcd.begin(21, 22);
	lcd.createChar(SB_SELECT, SELECT);
	lcd.backlight();
	lcd_print("AGRISYSTEM/" + HubID, LINE0, MIDDLE);
	lcd_print("IoT Labs - MIC@DTU", LINE1, MIDDLE);
	lcd_print("Starting", LINE2, MIDDLE);
}

void lcd_select(int col, int row) {
	static int old_col;
	static int old_row;

	lcd.setCursor(old_col, old_row);
	lcd.print(' ');

	old_col = col;
	old_row = row;

	lcd.setCursor(col, row);
	lcd.write(SB_SELECT);
}

void lcd_showMainMenu() {
	lcd.clear();
	lcd_print("HUB STATUS", LINE0, LEFT, 1);
	lcd_print("NODE ID 1", LINE1, LEFT, 1);
	lcd_print("NODE ID 2NODE ID 2NODE ID 2NODE ID 2", LINE2, LEFT, 1);
	//lcd_print("Vườn hoa sữa", LINE3, LEFT, 1);

}

void lcd_showTime(bool force = false) {
	if (force) {
		lcd.setCursor(17, 0);		lcd.print(dayShortStr(weekday()));
		lcd.setCursor(17, 1);		lcd.print(hour() < 10 ? " 0" + String(hour()) : " " + String(hour()));
		lcd.setCursor(17, 2);		lcd.print(minute() < 10 ? " 0" + String(minute()) : " " + String(minute()));
		lcd.setCursor(17, 3);		lcd.print(second() < 10 ? " 0" + String(second()) : " " + String(second()));
		return;
	}
	static ulong t = millis();
	if ((millis() - t) >= 1000) {
		t = millis();
		lcd_showTime(true);
	}
}
void show_symbol_select(int col, int row) {
	lcd.setCursor(col, row);
	lcd.write(SB_SELECT);
}
void clear_symbol_select(int col, int row) {
	lcd.setCursor(col, row);
	lcd.print(' ');
}


#pragma region Classes for LCD
//void lcd_showTime(bool force = false);
class lcd_currsor_coordinate {
public:
	int col = 0;
	int row = 0;
	lcd_currsor_coordinate(int _col = -1, int _row = -1) {
		if ((_col != -1) && (_row != -1)) {
			col = _col;
			row = _row;
		}
	}
	void print_detail(String name = "") {
		Dprintf("%s(%d,%d)\r\n", name.c_str(), col, row);
	}
};

//void lcd_print(String data, int line, int align = MIDDLE, int padding_left = 0);

class LCD_Frame_Main_Menu {
public:
	bool isExpand = false;
	int index_Device_Selected = 0;
	lcd_currsor_coordinate coordinate_symbol_device_selected;

	void select_next_device() {
		if (index_Device_Selected < (DevicesList.length() - 1)) {
			index_Device_Selected++;
			if (coordinate_symbol_device_selected.row < 3) {
				coordinate_symbol_device_selected.row++;
			}
		}
	}
	void select_previous_device() {
		if (index_Device_Selected > 0) {
			if (!((index_Device_Selected-- != 1) && (coordinate_symbol_device_selected.row == 1))) {
				if (coordinate_symbol_device_selected.row > 0) {
					coordinate_symbol_device_selected.row--;
				}
			}
		}
	}
	void render() {
		lcd.clear();
		if (DevicesList.at(0).ID == "") {
			lcd_print("Can not found", LINE0, LEFT, 1);
			lcd_print("HUB ID:  " + HubID, LINE1, LEFT, 1);
			lcd_print("ON SERVER.", LINE2, LEFT, 1);
			lcd_print("Please register!", LINE3, LEFT, 1);
			return;
		}
		String HubName = DevicesList.at(0).Name;
		HubName.toUpperCase();
		lcd_print(HubName, LINE0, LEFT, 3); //HUB NAME

		if (index_Device_Selected >= 0) {
			switch (coordinate_symbol_device_selected.row)
			{
			case 0:
				if ((DevicesList.length()) > 1) {
					lcd_print(String(index_Device_Selected + 1) + "." + DevicesList.at(index_Device_Selected + 1).Name, LINE1, LEFT, 1);
					if ((DevicesList.length()) > 2) {
						lcd_print(String(index_Device_Selected + 2) + "." + DevicesList.at(index_Device_Selected + 2).Name, LINE2, LEFT, 1);
						if ((DevicesList.length()) > 3) {
							lcd_print(String(index_Device_Selected + 3) + "." + DevicesList.at(index_Device_Selected + 3).Name, LINE3, LEFT, 1);
						}
					}
				}
				break;

			case 1:
				if ((DevicesList.length()) > 1) {
					lcd_print(String(index_Device_Selected) + "." + DevicesList.at(index_Device_Selected).Name, LINE1, LEFT, 1);
					if ((DevicesList.length()) > 2) {
						lcd_print(String(index_Device_Selected + 1) + "." + DevicesList.at(index_Device_Selected + 1).Name, LINE2, LEFT, 1);
						if ((DevicesList.length()) > 3) {
							lcd_print(String(index_Device_Selected + 2) + "." + DevicesList.at(index_Device_Selected + 2).Name, LINE3, LEFT, 1);
						}
					}
				}
				break;

			case 2:
				if ((DevicesList.length()) > 1) {
					lcd_print(String(index_Device_Selected - 1) + "." + DevicesList.at(index_Device_Selected - 1).Name, LINE1, LEFT, 1);
					if ((DevicesList.length()) > 2) {
						lcd_print(String(index_Device_Selected) + "." + DevicesList.at(index_Device_Selected).Name, LINE2, LEFT, 1);
						if ((DevicesList.length()) > 3) {
							lcd_print(String(index_Device_Selected + 1) + "." + DevicesList.at(index_Device_Selected + 1).Name, LINE3, LEFT, 1);
						}
					}
				}
				break;

			case 3:
				if ((DevicesList.length()) > 1) {
					lcd_print(String(index_Device_Selected - 2) + "." + DevicesList.at(index_Device_Selected - 2).Name, LINE1, LEFT, 1);
					if ((DevicesList.length()) > 2) {
						lcd_print(String(index_Device_Selected - 1) + "." + DevicesList.at(index_Device_Selected - 1).Name, LINE2, LEFT, 1);
						if ((DevicesList.length()) > 3) {
							lcd_print(String(index_Device_Selected) + "." + DevicesList.at(index_Device_Selected).Name, LINE3, LEFT, 1);
						}
					}
				}
				break;

			default:
				break;
			}

			show_symbol_select(coordinate_symbol_device_selected.col, coordinate_symbol_device_selected.row);
		}

		lcd_showTime(true);
	}
} Main_Menu;

class LCD_Frame_Detail_Hub {
public:
	int cursor_select = LINE0;
	void render() {
		lcd.clear();

		lcd_print("LIGHT  " + String(STT_LIGHT == STT_ON ? ON : OFF), LINE0, LEFT, 1);
		lcd_print("FAN    " + String(STT_FAN == STT_ON ? ON : OFF), LINE1, LEFT, 1);
		lcd_print("SPRAY  " + String(STT_SPRAY == STT_ON ? ON : OFF), LINE2, LEFT, 1);
		lcd_print("COVER  " + String(STT_COVER == STT_ON ? ON : OFF), LINE3, LEFT, 1);
		show_symbol_select(0, cursor_select);

		lcd_showTime(true);
	}
} Detail_Hub;

class LCD_Frame_Detail_Node {
private:
	const int PAGE_NODE_CONTROL = 0;
	const int PAGE_NODE_SENSOR = 1;

	String N_MANURE;
	String N_SPRAY;
	String N_LIGHT;
	String N_LED_MOSFET;

	float N_S_TEMP;
	float N_S_HUMI;
	float N_S_LIGHT;
	float N_S_PH;
	float N_S_EC;

public:
	int cursor_select = LINE0;
	int page_node = PAGE_NODE_CONTROL;

	String current_Node_ID;
	void get_Detail_Node(String NodeID) {
		current_Node_ID = NodeID;
		DynamicJsonBuffer jsBufferCommandGetDetailNode(500);
		JsonObject& jsCommandGetDetailNode = jsBufferCommandGetDetailNode.createObject();

		jsCommandGetDetailNode[MES_ID] = String(micros());
		jsCommandGetDetailNode[HUB_ID] = HubID;
		jsCommandGetDetailNode[SOURCE] = HubID;
		jsCommandGetDetailNode[DEST] = NodeID;
		jsCommandGetDetailNode[TIMESTAMP] = String(now());
		jsCommandGetDetailNode[CMD_T] = int(GET_DATA_GARDEN_NODE);

		String strGetDetailNode;
		jsCommandGetDetailNode.printTo(strGetDetailNode);
		Rprintln(strGetDetailNode);
		Dprintf("RF <<< [%d]\r\n", strGetDetailNode.length());
		Dprintln(strGetDetailNode);
		delay(10);
	}
	void render() {
		lcd.clear();

		if (page_node == PAGE_NODE_CONTROL) {
			lcd_print("MANURE   " + N_MANURE, LINE0, LEFT, 1);
			lcd_print("SPRAY    " + N_SPRAY, LINE1, LEFT, 1);
			lcd_print("LIGHT    " + N_LIGHT, LINE2, LEFT, 1);
			lcd_print("LED_MOS  " + N_LED_MOSFET.substring(2) + "%", LINE3, LEFT, 1);
			show_symbol_select(0, cursor_select);
		}
		else if (page_node == PAGE_NODE_SENSOR) {

		}

		lcd_showTime(true);
	}

	void refresh_Detail_Node(JsonObject& nodeData) {
		N_MANURE = nodeData[MANURE].as<String>();
		N_SPRAY = nodeData[SPRAY].as<String>();
		N_LIGHT = nodeData[LIGHT].as<String>();
		N_LED_MOSFET = nodeData[LED_MOSFET].as<String>();

		N_S_TEMP = nodeData[S_TEMP].as<String>().toFloat();
		N_S_HUMI = nodeData[S_HUMI].as<String>().toFloat();
		N_S_LIGHT = nodeData[S_LIGHT].as<String>().toFloat();
		N_S_PH = nodeData[S_PH].as<String>().toFloat();
		N_S_EC = nodeData[S_EC].as<String>().toFloat();
	}

	void reset_temp_detail() {
		N_MANURE = "";
		N_SPRAY = "";
		N_LIGHT = "";
		N_LED_MOSFET = "";

		N_S_TEMP = 0.0;
		N_S_HUMI = 0.0;
		N_S_LIGHT = 0.0;
		N_S_PH = 0.0;
		N_S_EC = 0.0;
	}
} Detail_Node;

class LCD_Frame_Detail_Envi {
public:
	void render() {
		lcd.clear();
		lcd_showTime(true);
	}
} Detail_Envi;

class LCD_Frame_Detail_Tank {
public:
	void render() {
		lcd.clear();
		lcd_showTime(true);
	}
} Detail_Tank;

class LCD_Frame_Class {
public:
	//LCD_Frame_Main_Menu Main_Menu;
	LCD_Pages current_page = LCD_PAGE_MAIN_MENU;

	void update_Frame() {
		int button = button_read();
		if (button > 0) {
			switch (current_page)
			{
			case LCD_PAGE_MAIN_MENU:
				switch (button)
				{
				case BT_NOBUTTON:
					break;
				case BT_DOWN:
					Main_Menu.select_next_device();
					Main_Menu.render();
					break;

				case BT_UP:
					Main_Menu.select_previous_device();
					Main_Menu.render();
					break;

				case BT_LEFT:
				case BT_BACK:
					break;

				case BT_RIGHT:
				case BT_ENTER:
					switch (DevicesList.at(Main_Menu.index_Device_Selected).Type)
					{
					case UNKNOWN:
						break;
					case GARDEN_HUB:
						current_page = LCD_PAGE_DETAIL_HUB;
						Detail_Hub.render();
						break;
					case GARDEN_NODE:
						current_page = LCD_PAGE_DETAIL_NODE;
						Detail_Node.cursor_select = LINE0;
						Detail_Node.get_Detail_Node(DevicesList.at(Main_Menu.index_Device_Selected).ID);
						Detail_Node.render();
						break;
					case ENVIROMENT_MONITOR:
						current_page = LCD_PAGE_DETAIL_ENVI;
						Detail_Envi.render();
						break;
					case TANK_CONTROLER:
						current_page = LCD_PAGE_DETAIL_TANK;
						Detail_Tank.render();
						break;
					default:
						break;
					}

					break;
				default:
					break;
				}

				break;

			case LCD_PAGE_DETAIL_HUB:
				switch (button)
				{
				case BT_NOBUTTON:
					break;
				case BT_DOWN:
					if (Detail_Hub.cursor_select < LINE3) {
						Detail_Hub.cursor_select++;
					}
					Detail_Hub.render();
					break;

				case BT_UP:
					if (Detail_Hub.cursor_select > LINE0) {
						Detail_Hub.cursor_select--;
					}
					Detail_Hub.render();
					break;

				case BT_LEFT:
				case BT_BACK:
					current_page = LCD_PAGE_MAIN_MENU;
					Main_Menu.render();
					break;
				case BT_RIGHT:
				case BT_ENTER:
					extern void control_relay_hub(int HPIN, String STT, bool publish = false, bool isAppControl = true);
					switch (Detail_Hub.cursor_select)
					{
					case LINE0: //LIGHT
						STT_LIGHT = abs(1 - STT_LIGHT); //toggle
						control_relay_hub(HPIN_LIGHT, STT_LIGHT == STT_ON ? ON : OFF, false, false);
						break;
					case LINE1: //FAN
						STT_FAN = abs(1 - STT_FAN); //toggle
						control_relay_hub(HPIN_FAN, STT_FAN == STT_ON ? ON : OFF, false, false);
						break;
					case LINE2: //SPRAY
						STT_SPRAY = abs(1 - STT_SPRAY); //toggle
						control_relay_hub(HPIN_SPRAY, STT_SPRAY == STT_ON ? ON : OFF, false, false);
						break;
					case LINE3: //COVER
						STT_COVER = abs(1 - STT_COVER); //toggle
						control_relay_hub(HPIN_COVER, STT_COVER == STT_ON ? ON : OFF, false, false);
						break;
					default:
						break;
					}
					upload_relay_hub_status();
					//no need Detail_Hub.render. Render in upload_relay_hub_status()
					break;
				default:
					break;
				}
				break;

			case LCD_PAGE_DETAIL_NODE:
				switch (button)
				{
				case BT_NOBUTTON:
					break;
				case BT_DOWN:
					if (Detail_Node.cursor_select < LINE3) {
						Detail_Node.cursor_select++;
					}
					Detail_Node.render();
					break;
				case BT_UP:
					if (Detail_Node.cursor_select > LINE0) {
						Detail_Node.cursor_select--;
					}
					Detail_Node.render();
					break;
				case BT_LEFT:
					break;
				case BT_BACK:
					current_page = LCD_PAGE_MAIN_MENU;
					Main_Menu.render();
					break;
				case BT_RIGHT:
					Detail_Node.get_Detail_Node(Detail_Node.current_Node_ID);
					break;
				case BT_ENTER:
					break;
				default:
					break;
				}
				break;

			case LCD_PAGE_DETAIL_ENVI:
				switch (button)
				{
				case BT_NOBUTTON:
					break;
				case BT_DOWN:
					break;
				case BT_UP:
					break;
				case BT_LEFT:
					break;
				case BT_BACK:
					current_page = LCD_PAGE_MAIN_MENU;
					Main_Menu.render();
					break;
				case BT_RIGHT:
				case BT_ENTER:
					break;
				default:
					break;
				}
				break;
			case LCD_PAGE_DETAIL_TANK:
				switch (button)
				{
				case BT_NOBUTTON:
					break;
				case BT_DOWN:
					break;
				case BT_UP:
					break;
				case BT_LEFT:
					break;
				case BT_BACK:
					current_page = LCD_PAGE_MAIN_MENU;
					Main_Menu.render();
					break;
				case BT_RIGHT:
				case BT_ENTER:
					break;
				default:
					break;
				}
				break;
			}
		}
	}
} LCD_Frame;
#pragma endregion

#pragma endregion


void upload_relay_hub_status() {
	if (LCD_Frame.current_page == LCD_PAGE_DETAIL_HUB) {
		Detail_Hub.render();
	}

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
	mqtt_publish(MQTT_TOPIC_MAIN + "/" RESPONSE, dataRelayHub, true);
}
void upload_relay_changelogs(String action_name, bool isAppControl = true) {
	DynamicJsonBuffer jsBufferRelayChangelog(500);
	JsonObject& jsRelayChangelog = jsBufferRelayChangelog.createObject();

	jsRelayChangelog[MES_ID] = String(micros());
	jsRelayChangelog[HUB_ID] = HubID;
	jsRelayChangelog[SOURCE] = HubID;
	jsRelayChangelog[DEST] = SERVER;
	jsRelayChangelog[TIMESTAMP] = String(now());
	jsRelayChangelog[CMD_T] = int(UPDATE_ACTION_LOGS);
	jsRelayChangelog[HUB_CODE] = HubID;
	jsRelayChangelog[ACTION_NAME] = action_name;
	jsRelayChangelog[ACTION_FROM] = isAppControl ? APP : HubID;
	jsRelayChangelog[ACTION_TO] = HubID;

	String dataRelayChangelog;
	jsRelayChangelog.printTo(dataRelayChangelog);
	mqtt_publish(MQTT_TOPIC_MAIN + "/LOGS", dataRelayChangelog, true);
}

void control_relay_hub(int HPIN, String STT, bool publish = false, bool isAppControl = true) {
	Dprintf("Turn pin %d %s\r\n", HPIN, STT == ON ? "on" : "off");

	if (HPIN == HPIN_LIGHT) {
		if (STT == ON) {
			STT_LIGHT = STT_ON;
			digitalWrite(HPIN, true);
			upload_relay_changelogs("LIGHT ON", isAppControl);
		}
		else if (STT == OFF) {
			STT_LIGHT = STT_OFF;
			digitalWrite(HPIN, false);
			upload_relay_changelogs("LIGHT OFF", isAppControl);
		}
	}
	else if (HPIN == HPIN_FAN) {
		if (STT == ON) {
			STT_FAN = STT_ON;
			digitalWrite(HPIN, true);
			upload_relay_changelogs("FAN ON", isAppControl);
		}
		else if (STT == OFF) {
			STT_FAN = STT_OFF;
			digitalWrite(HPIN, false);
			upload_relay_changelogs("FAN OFF", isAppControl);
		}
	}
	else if (HPIN == HPIN_SPRAY) {
		if (STT == ON) {
			STT_SPRAY = STT_ON;
			digitalWrite(HPIN, true);
			upload_relay_changelogs("SPRAY ON", isAppControl);
		}
		else if (STT == OFF) {
			STT_SPRAY = STT_OFF;
			digitalWrite(HPIN, false);
			upload_relay_changelogs("SPRAY OFF", isAppControl);
		}
	}
	else if (HPIN == HPIN_COVER) {
		if (STT == ON) {
			STT_COVER = STT_ON;
			digitalWrite(HPIN_COVER_OPEN, true);
			digitalWrite(HPIN_COVER_CLOSE, false);
			upload_relay_changelogs("COVER ON", isAppControl);
		}
		else if (STT == OFF) {
			STT_COVER = STT_OFF;
			digitalWrite(HPIN_COVER_OPEN, false);
			digitalWrite(HPIN_COVER_CLOSE, true);
			upload_relay_changelogs("COVER OFF", isAppControl);
		}
		else if (STT == MID) {
			STT_COVER = STT_OFF;
			digitalWrite(HPIN_COVER_OPEN, false);
			digitalWrite(HPIN_COVER_CLOSE, false);
			upload_relay_changelogs("COVER STOP", isAppControl);
		}
	}

	if (publish) {
		upload_relay_hub_status();
	}
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
	Dprintf("\r\nRF >>> [%d]\r\n", rf_Message.length());
	Dprintln(rf_Message);
	//Rprint(F("RF>> "));
	//Rprintln(rf_Message);
	DynamicJsonBuffer jsonBufferNodeData(500);
	JsonObject& nodeData = jsonBufferNodeData.parseObject(rf_Message);

	static byte number_rf_fail = 0;
	if (!nodeData.success()) {
		Dprintln(F("#ERR rf_Message invalid"));
		//Dprintln(rf_Message);
		if (++number_rf_fail > 5) {
			number_rf_fail = 0;
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
	else {
		number_rf_fail = 0;
	}

	String _hubID = nodeData[HUB_ID].as<String>();
	String _DEST = nodeData[DEST].as<String>();
	String _SOURCE = nodeData[SOURCE].as<String>();
	nodeData[TIMESTAMP] = String(now());

	if (_hubID == HubID) {

		//update on lcd
		if (_SOURCE == Detail_Node.current_Node_ID) {
			Detail_Node.reset_temp_detail();
			Detail_Node.refresh_Detail_Node(nodeData);
			Detail_Node.render();
		}


		String nodeDataString;
		if (_DEST == HubID) {
			nodeData[DEST] = SERVER;
			nodeData.printTo(nodeDataString);
		}
		else {
			nodeDataString = rf_Message;
		}
		mqtt_publish(MQTT_TOPIC_MAIN + "/" RESPONSE "/" + _SOURCE, nodeDataString, true);
	}
}

void handle_serial() {
	if (Serial.available()) {
		String s = Serial.readString();
		Dprintln(F("Serial >>> "));
		Dprintln(s);
		Rprintln(s);
		Dprintf("RF <<< [%d]\r\n", s.length());
		Dprintln(s);
		delay(10);
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
	static const unsigned long debound_time = 2;
	static int last_button = BT_NOBUTTON;
	if ((millis() - t) > debound_time) {
		t = millis();
		for (int i = total - 1; i > 0; i--) {
			pre[i] = pre[i - 1];
		}
		pre[0] = map_value_to_button(analogRead(HPIN_BUTTON));
		int err = 0;
		for (int i = 1; i < total; i++) {
			if (pre[i] != pre[0]) {
				err++;
			}
		}
		if ((err == 0) && (last_button != pre[0])) {
			if (last_button == BT_NOBUTTON) {
				Dprintln();
				switch (pre[0])
				{
				case BT_LEFT:
					Dprint(F("BT_LEFT"));
					break;
				case BT_DOWN:
					Dprint(F("BT_DOWN"));
					break;
				case BT_RIGHT:
					Dprint(F("BT_RIGHT"));
					break;
				case BT_UP:
					Dprint(F("BT_UP"));
					break;
				case BT_BACK:
					Dprint(F("BT_BACK"));
					break;
				case BT_ENTER:
					Dprint(F("BT_ENTER"));
					break;
				default:
					break;
				}
				Dprint(" ");
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



#pragma region WiFi Init
void smart_config() {
	digitalWrite(LED_BUILTIN, LOW);
	bool sttled = false;
	Dprintln(F("SmartConfig started."));
	unsigned long t = millis();
	WiFi.beginSmartConfig();
	while (1) {
		sttled = !sttled;
		digitalWrite(LED_BUILTIN, sttled);
		delay(200);
		if (WiFi.smartConfigDone()) {
			Dprintln(F("SmartConfig: Success"));
			Dprint(F("RSSI: "));
			Dprintln(WiFi.RSSI());
			WiFi.printDiag(Serial);
			//WiFi.stopSmartConfig();
			break;
		}

		if ((millis() - t) > (3 * 60000)) {
			Dprintln(F("ESP restart"));
			ESP.restart();
		}
	}
	if (WiFi.waitForConnectResult() == WL_CONNECTED)
	{
		Dprintln(F("connected\n"));
	}
	else {
		smart_config();
	}
	digitalWrite(LED_BUILTIN, HIGH);
	Dprint(F("IP: "));
	Dprintln(WiFi.localIP());
}
void wifi_init() {
	Dprintln(F("\r\nConnecting to WiFi"));
	if (WiFi.isConnected()) {
		return;
	}
	WiFi.begin("IoT Wifi", "mic@dtu12345678()");
	WiFi.waitForConnectResult();
	ulong t = millis();
	while (1) {
		if (WiFi.isConnected()) {
			break;
		}
		if ((millis() - t) > 60000) {
			ESP.deepSleep(10000);
		}
		delay(1);
	}
	Dprint(F("Local IP: "));
	Dprintln(WiFi.localIP());

	return;

	//============================================================
	WiFi.setAutoConnect(true);
	WiFi.setAutoReconnect(true);
	WiFi.mode(WIFI_STA);

	String WiFiDiag;
	WiFi.printDiag((Print&)WiFiDiag);

	Dprintln(WiFiDiag);
	Dprintln(F("\nConnecting..."));

	if (WiFi.waitForConnectResult() == WL_CONNECTED)
	{
		digitalWrite(LED_BUILTIN, HIGH);
		Dprintln(F("connected\n"));
		Dprintln(WiFi.localIP());
	}
	else
	{
		smart_config();
	}
}
#pragma endregion


#pragma region HTTP
String http_request(String host, uint16_t port = 80, String url = "/") {
	Dprintln("\r\nGET " + host + ":" + String(port) + url);
	WiFiClient client;
	client.setTimeout(100);
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


#pragma region MQTT

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
			Dprintf("RF <<< [%d]\r\n", json.length());
			//ulong t = millis();
			//Dprintln(millis() - t);
			//t = millis();
			Dprintln(json);
			Dflush();
			delay(10); //!important
		}
	}
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

	//StaticJsonBuffer<500> jsonBufferServer;
	//JsonObject& nodeLib = jsonBufferServer.parseObject(json);
	//if (!nodeLib.success()) {
	//	Dprintln(F("#ERR json invalid"));
	//	Dprintln();
	//	return;
	//}
	//String TRAYID = nodeLib["TRAY_ID"].asString();
	//String HUBCODE = nodeLib["HUB_CODE"].asString();
	//String TRAYNAME = nodeLib["TRAY_NAME"].asString();
	//int LIGHTMIN = nodeLib["LIGHT_MIN"].as<int>();
	//int LIGHTMAX = nodeLib["LIGHT_MAX"].as<int>();
	//int HUMIMIN = nodeLib["HUMI_MIN"].as<int>();
	//int HUMIMAX = nodeLib["HUMI_MAX"].as<int>();
	//int TEMPMIN = nodeLib["TEMP_MIN"].as<int>();
	//int TEMPMAX = nodeLib["TEMP_MAX"].as<int>();
	//int AUTOSTATUS = nodeLib["AUTO_STATUS"].as<int>();
	//int INTERVALUPDATE = nodeLib["INTERVAL_UPDATE"].as<int>();

	Rprintln(json);
	Dprintf("RF <<< [%d]\r\n", json.length());
	Dprintln(json);
	delay(10);
}

void mqtt_callback(char* topic, uint8_t* payload, unsigned int length) {
	//ulong t = millis();

	//Dprint(F("\r\n#1 FREE RAM : "));
	//Dprintln(ESP.getFreeHeap());

	String topicStr = topic;
	Dprintf("\r\nMQTT >>> [%d] %s\r\n", length, topicStr.c_str());
	//Dprintln(topicStr);
	//Dprint(F("Message arrived: "));
	//Dprint(topicStr);
	//Dprint(F("["));
	//Dprint(length);
	//Dprintln(F("]"));

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
	if (topicStr.startsWith(MQTT_TOPIC_MAIN + "/" REQUEST))
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
	ulong t = millis();
	while (!mqtt_client.connected()) {
		Dprint(F("Attempting MQTT connection..."));
		//boolean connect(const char* id, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage);
		String h_offline = "{\"HUB_ID\":\"" + HubID + "\",\"STATUS\":\"OFFLINE\"}";
		String topic_HUBSTATUS = MQTT_TOPIC_MAIN + "/STATUS";
		if (mqtt_client.connect(HubID.c_str(), mqtt_user, mqtt_password, topic_HUBSTATUS.c_str(), 0, true, h_offline.c_str())) {
			Dprintln(F("connected"));
			String h_online = "{\"HUB_ID\":\"" + HubID + "\",\"STATUS\":\"ONLINE\"}";
			mqtt_client.publish(topic_HUBSTATUS.c_str(), h_online.c_str(), true);
			mqtt_client.subscribe((MQTT_TOPIC_MAIN + "/" REQUEST "/#").c_str());
			String libs = MQTT_TOPIC_MAIN + "/LIBS/#";
			mqtt_client.subscribe(libs.c_str());
			String notify = MQTT_TOPIC_MAIN + "/NOTIFY/HUB";
			mqtt_client.subscribe(notify.c_str());
		}
		else {
			if ((millis() - t) > 120000) {
				ESP.deepSleep(10000);
			}
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
	Dprintf("\r\nMQTT <<< [%d] %s\r\n", payload.length(), topic.c_str());
	//Dprintln(topic);
	Dprintln(payload);
	//Dprintln();

	digitalWrite(LED_STATUS, LOW);
	bool ret = mqtt_client.publish(topic.c_str(), payload.c_str(), retain);
	digitalWrite(LED_STATUS, HIGH);
	return ret;
}
#pragma endregion



#pragma region TASKS
void updateTimeStamp(unsigned long interval = 0) {
	static unsigned long t_pre_update = 0;
	static bool wasSync = false;
	if (interval == 0) {
		{
			String strTimeStamp = http_request("date.jsontest.com");
			Dprintln(strTimeStamp);
			DynamicJsonBuffer timestamp(500);
			JsonObject& jsTimeStamp = timestamp.parseObject(strTimeStamp);
			if (jsTimeStamp.success()) {
				String tt = jsTimeStamp["milliseconds_since_epoch"].asString();
				tt = tt.substring(0, tt.length() - 3);
				long ts = tt.toInt();
				if (ts > 1000000000) {
					t_pre_update = millis();
					wasSync = true;
					setTime(ts);
					adjustTime(7 * SECS_PER_HOUR);
					Dprintln(F("Time Updated\r\n"));
					lcd_showTime();
					return;
				}
			}
		}

		String strTimeStamp = http_request("mic.duytan.edu.vn", 88, "/api/GetUnixTime");
		Dprintln(strTimeStamp);
		DynamicJsonBuffer timestamp(500);
		JsonObject& jsTimeStamp = timestamp.parseObject(strTimeStamp);
		if (jsTimeStamp.success()) {
			time_t ts = String(jsTimeStamp["UNIX_TIME"].asString()).toInt();
			if (ts > 1000000000) {
				t_pre_update = millis();
				wasSync = true;
				setTime(ts);
				adjustTime(7 * SECS_PER_HOUR);
				Dprintln(F("Time Updated\r\n"));
				lcd_showTime();
				return;
			}
		}
	}
	else {
		if ((millis() - t_pre_update) > interval) {
			updateTimeStamp();
		}
	}
	if (!wasSync) {
		updateTimeStamp();
	}
}

int wifi_quality() {
	int dBm = WiFi.RSSI();
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
		jsHubHardwareStatus[CMD_T] = (int)UPDATE_HUB_HARDWARE_STATUS;
		jsHubHardwareStatus[WIFI_SIGNAL] = String(wifi_quality());
		jsHubHardwareStatus[TEMP_INTERNAL] = String(temperatureRead(), 2);

		String strHubHardwareStatus;
		jsHubHardwareStatus.printTo(strHubHardwareStatus);

		String dataRelayHub;
		mqtt_publish(MQTT_TOPIC_MAIN + "/" RESPONSE, strHubHardwareStatus, true);
	}
}

bool update_tray_list(bool force = false) {
	static bool updateSuccess = false;
	if ((!force) && updateSuccess) {
		return true;
	}

	String _devices_List = http_request("mic.duytan.edu.vn", 88, "/api/GetAllHubDevices/HUB_ID=" + HubID);

	DynamicJsonBuffer jsonBufferDevicesList(10000);
	JsonObject& DevicesListJsObj = jsonBufferDevicesList.parseObject(_devices_List);
	if (DevicesListJsObj.success()) {
		DevicesList.clear();
		String hid = DevicesListJsObj[HUB_ID].asString();
		String hname = DevicesListJsObj[HUB_NAME].asString();
		Devices_Info _Hub_Info(hid, hname);
		DevicesList.push_front(_Hub_Info);
		if (hid == "") {
			return false;
		}

		int  total_devices = DevicesListJsObj["TOTAL_DEVICES"].as<int>();
		Dprintf("TOTAL_DEVICES %d\r\n", total_devices);

		if (total_devices > 0) {
			JsonArray& DevicesListJsArr = DevicesListJsObj["DEVICE_LIST"];
			for (int i = 0; i < total_devices; i++) {
				JsonObject& DeviceInfoJsObj = DevicesListJsArr[i];
				String did = DeviceInfoJsObj["DEVICE_ID"].asString();
				String dname = DeviceInfoJsObj["DEVICE_NAME"].asString();
				//Dprintf("DEVICE_NAME[%d] %s", i, dname.c_str());
				Devices_Info _Device_Info(did, dname);

				DevicesList.push_back(_Device_Info);
			}
		}
		updateSuccess = true;
		return true;
	}
	return false;
}
#pragma endregion


void setup()
{
	delay(1000);
	hardware_init();
	lcd_init();

	wifi_init();
	updateTimeStamp();

	mqtt_init();
	lcd_print("WiFi connected", LINE2, MIDDLE);
	lcd_print(WiFi.localIP().toString(), LINE3, MIDDLE);

	lcd_showMainMenu();

	if (update_tray_list()) {
		Dprintln("DevicesList_length = " + String(DevicesList.length()));
		//LCD_Frame.Main_Menu.init(DevicesList.at(0), lcd_currsor_coordinate(0, 0));
		LCD_Frame.current_page = LCD_PAGE_MAIN_MENU;
		Main_Menu.render();
	}
	else {
		Dprintln("ERR#4353 No Hub on Server");
		lcd.clear();
		lcd_print("AGRISYSTEM-IoTLab", LINE0, MIDDLE, 0);
		lcd_print(String("HID = ") + HubID, LINE2, MIDDLE, 0);
	}

	RF.begin(RF_BAUDRATE);
	RF.setTimeout(200);
}

void loop()
{
	IDLE
		updateHubHardwareStatus(30000);
	IDLE
		updateTimeStamp(60000);
	IDLE
		mqtt_loop();
	IDLE
		handle_rf_communicate();
	IDLE
		handle_serial();
	IDLE
		LCD_Frame.update_Frame();
	IDLE
		lcd_showTime();
	IDLE
		if (DevicesList.length() == 0) {
			update_tray_list();
		}
}