#define IGNORE_THIS_FILE
#ifndef IGNORE_THIS_FILE

#define API_VERSION  "0.1.16"
/*
	v0.1.14	add COMMAND_TYPE::ADD_NEW_TRAY
*/

//======================================================================================================================================

Cách đặt ID cho các thiết bị: tất cả kí tự đề là chữ HOA
	GARDEN_HUB		   : HXXXXX
	GARDEN_NODE		   : NXXXXX
	ENVIROMENT_MONITOR : EXXXXX
	TANK_CONTROLER	   : TXXXXX

Trong đó kí tự đầu tiên đại diện cho loại thiết bị, các kí tự còn lại mã xác định ID.

/*
-------------------------------------------------------------------------------------------------------------------
Thêm 4 trường 
	MES_ID	   : Message ID
	SOURCE	   : ID nguồn gửi
	DEST	   : ID đích gửi
	TIMESTAMP  : seconds from epoch
vào mỗi gói tín, sau này dễ dàng trong việc mở rộng theo nhiều hướng.
-------------------------------------------------------------------------------------------------------------------
 */

// Ý tưởng chung là nếu Hardware Side nhận được command CONTROL hoặc GET_DATA thì sẽ gửi trả lại message là UPDATE_DATA.
// Ngoài ra Hardware Side còn định kỳ gửi message UPDATE_DATA sau mỗi INTERVAL_UPDATE trong Library AGRISYSTEM/<HubID>/LIBS/<HardwareID>

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
	UPDATE_GARDEN_DEVICES						//without retain
};

enum NODE_TYPE {
	GARDEN_HUB = 0,
	GARDEN_NODE,
	ENVIROMENT_MONITOR,
	TANK_CONTROLER
};

enum NOTIFICATION_TYPE {
	NO_NOTIFY = 0,

	HUB_ONLINE,
	HUB_OFFLINE,

	LIGHT_TOO_HIGH,
	LIGHT_TOO_LOW,
	
	TEMP_GARDEN_TOO_HIGH,
	TEMP_GARDEN_TOO_LOW,
	HUMI_GARDEN_TOO_LIGH,
	HUMI_GARDEN_TOO_LOW,

	TEMP_SOIL_TOO_LOW,
	TEMP_SOIL_TOO_HIGH,
	HUMI_SOIL_TOO_LOW,
	HUMI_SOIL_TOO_HIGH,
	
	RAIN,
	WINDY_TOO_STRONG,
	WATER_TOO_LOW,
	WATER_A_LOW,
	WATER_B_LOW,

	ERROR_SENSOR_TEMP,
	ERROR_SENSOR_HUMI,
	ERROR_SENSOR_LIGHT,
	ERROR_SENSOR_PH,
	ERROR_SENSOR_EC
};


/*
	GARDEN_HUB
		"LIGHT" ----- Đèn
			"ON"	
			"OFF"
		"FAN" ------- Quạt thông gió
			"ON"
			"OFF"
		"SPRAY" ------ Bơm phun sương
			"ON"
			"OFF"
		"COVER" ----- Mái che
			"ON"
			"OFF"
			"STOP" --	Request
			"MID" --- Response
		
	"GARDEN_NODE"
		"MANURE" ---- Bơm bón phân
			"ON"
			"OFF"
		"SPRAY" ----- Bơm tưới nước
			"ON"
			"OFF"
		"LIGHT" ----- Đèn chiếu sáng
			"ON"
			"OFF"
		"LED_MOSFET  Đèn quang hợp, có thể điều chỉnh độ sáng
			"PC<PERCENT>" : "PC45", "PC78"
		"S_TEMP"
			"<float>"
		"S_HUMI"
			"<float>"
		"S_LIGHT"
			"<float>"
		"S_PH"
			"<float>"
		"S_EC"
			"<float>"
			
	"ENVIROMENT_MONITOR"
		"TEMP"
			"(float)inCelsius"
		"HUMI"
			"(float)inPercent"
		"LIGHT"
			"(float)inLux"
		"RAIN"
			"YES"
			"NO"
		"WIND"
			"YES"
			"NO"
			
	"TANK_CONTROLER"
		"WATER_IN"
			"ON"
			"OFF"
		"WATER_OUT"
			"ON"
			"OFF"
		"WATER_LEVEL"
			"(float)inPercent"
		"WATER_HIGH"
			"YES"
			"NO"
		"WATER_LOW"
			"YES"
			"NO"
*/

#region GARDEN_HUB
CONTROL_GARDEN_HUB
	Topic: "AGRISYSTEM/<HubID>/REQUEST"
	{
		"MES_ID"	 : "<string>",
		"HUB_ID"	 : "<string>",
		"SOURCE"	 : "<string>",
		"DEST"		 : "<string>", 
		"TIMESTAMP"	 : "<long>",
		"CMD_T"		 : CONTROL_GARDEN_HUB,
		"LIGHT"		 : "ON",
		"FAN"		 : "ON",
		"SPRAY"		 : "ON",
		"COVER"		 : "OFF" //sẽ có 3 giá trị cho cái mái che này: ON, OFF, STOP. ON = OPEN, OFF = CLOSE
	}
		
GET_DATA_GARDEN_HUB
	Topic: "AGRISYSTEM/<HubID>/REQUEST"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"TIMESTAMP"  : "",
		"CMD_T"		 : GET_DATA_GARDEN_HUB
	}
		
UPDATE_DATA_GARDEN_HUB
	Topic: "AGRISYSTEM/<HubID>/RESPONSE"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"TIMESTAMP"  : "",
		"CMD_T"		 : UPDATE_DATA_GARDEN_HUB,
		"LIGHT"		 : "ON",
		"FAN"		 : "ON",
		"SPRAY"		 : "ON",
		"COVER"		 : "OFF"	// ON, OFF or MID. MID là ở giữa, ko đụng đến công tắc hành trình ở biên
	}
#end region

#region GARDEN_NODE
CONTROL_GARDEN_NODE
	Topic: "AGRISYSTEM/<HubID>/REQUEST/<NodeID>"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"TIMESTAMP"  : "",
		"CMD_T"		 : CONTROL_GARDEN_NODE,
		"MANURE"	 : "ON",
		"SPRAY"		 : "ON",
		"LIGHT"		 : "OFF",
		"LED_MOSFET" : "PC45"
	}
GET_DATA_GARDEN_NODE
	Topic: "AGRISYSTEM/<HubID>/REQUEST/<NodeID>"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"TIMESTAMP"  : "",
		"CMD_T"		 : GET_DATA_GARDEN_NODE
	}

UPDATE_DATA_GARDEN_NODE
	Topic: "AGRISYSTEM/<HubID>/RESPONSE/<NodeID>"
	
	#Trồng đất
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"TIMESTAMP"  : "",
		"CMD_T"		 : UPDATE_DATA_GARDEN_NODE,
		"ACTION_FROM" : "HIC0001", //chỉ có khi response từ CONTROL_GARDEN_NODE
		"MANURE"	 : "ON",
		"SPRAY"		 : "ON",
		"LIGHT"		 : "OFF",
		"LED_MOSFET" : "PC45",
		
		"S_TEMP"	 : "<float>",
		"S_HUMI"	 : "<float>",
		"S_LIGHT"	 : "<float>",
		"S_PH"		 : "<float>",
		"S_EC"		 : "<float>"
	}

	#Thủy canh
	{
		"MES_ID"	 : "",
		"HUB_ID" : "",
		"SOURCE" : "",
		"DEST" : "",
		"TIMESTAMP" : "",
		"CMD_T" : UPDATE_DATA_GARDEN_NODE,
		"ACTION_FROM" : "HIC0001",	//chỉ có khi response từ CONTROL_GARDEN_NODE
		"SPRAY" : "ON",				//hiển thị trên giao diện là FAN 
		"LIGHT" : "OFF",
		"LED_MOSFET" : "PC45",

		"S_TEMP" : "<float>",
		"S_HUMI" : "<float>",
		"S_LIGHT" : "<float>",
		"S_PH" : "<float>",
		"S_EC" : "<float>"
	}
#end region

#region ENVIROMENT_MONITOR
CONTROL_ENVIROMENT_MONITOR
	none
	
GET_DATA_ENVIROMENT_MONITOR
Topic: "AGRISYSTEM/<HubID>/REQUEST/<ENVIROMENT_ID>"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"TIMESTAMP" : "",
		"DEST"		 : "<ENVIROMENT_ID>",
		"TIMESTAMP"  : "",
		"CMD_T"		 : GET_DATA_ENVIROMENT_MONITOR
	}
UPDATE_DATA_ENVIROMENT_MONITOR
	Topic: "AGRISYSTEM/<HubID>/RESPONSE/<ENVIROMENT_ID>"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"TIMESTAMP"  : "",
		"CMD_T"		 : UPDATE_DATA_ENVIROMENT_MONITOR,
		
		"S_TEMP"	 : "<float>",
		"S_HUMI"	 : "<float>",
		"S_LIGHT"	 : "<float>",
		"S_RAIN"	 : "<float>",
		"S_WIND"	 : "<float>"
	}

#end region

#region CONTROL_TANK_CONTROLER
CONTROL_TANK_CONTROLER
	Topic: "AGRISYSTEM/<HubID>/REQUEST/<TANK_ID>"
	{
		"MES_ID"	 : "<string>",
		"HUB_ID"	 : "<string>",
		"SOURCE"	 : "<string>",
		"DEST"		 : "<string>",
		"TIMESTAMP" : "",
		"CMD_T"		 : CONTROL_TANK_CONTROLER,
		"WATER_IN"	 : "ON",
		"WATER_OUT"	 : "OFF"
	}
GET_DATA_TANK_CONTROLER
	Topic: "AGRISYSTEM/<HubID>/REQUEST/<TANK_ID>"
	{
		"MES_ID"	 : "<string>",
		"HUB_ID"	 : "<string>",
		"SOURCE"	 : "<string>",
		"DEST"		 : "<string>",
		"TIMESTAMP" : "",
		"CMD_T"		 : GET_DATA_TANK_CONTROLER
	}

UPDATE_DATA_TANK_CONTROLER
	Topic: "AGRISYSTEM/<HubID>/RESPONSE/<TANK_ID>"
	{
		"MES_ID"	 : "<string>",
		"HUB_ID"	 : "<string>",
		"SOURCE"	 : "<string>",
		"DEST"		 : "<string>",
		"TIMESTAMP"  : "",
		"CMD_T"		 : UPDATE_DATA_TANK_CONTROLER,
		
		"WATER_IN"	 : "ON",
		"WATER_OUT"	 : "OFF",
		
		"WATER_LEVEL": "<DISTANCE in cm>",
		"WATER_HIGH" : "YES",
		"WATER_LOW"	 : "NO"
	}
#end region


#region LIBRARIES

GARDEN_NODE
- HUB sẽ subscribe vào topic "AGRISYSTEM/<HubID>/LIBS/#"
- Thông tin về thư viện của từng TRAY sẽ được APP or SERVER publish lên topic "AGRISYSTEM/<HubID>/LIBS/<TrayID>" with retain
Nội dung tùy 
   {
	  "MES_ID"		: "<string>",
	  "HUB_ID"		: "<string>",
	  "SOURCE"		: "<string>",
	  "DEST"		: "<string>",
	  "TIMESTAMP"	: "<long>",
	  "CMD_T"		: LIBS_GARDEN_NODE,

      "TRAY_NAME"	: "Cải", //bỏ dấu please
      // "TRAY_IMAGE":"https://i.imgur.com/zLHjhzW.jpg",
      // "TRAY_CREATED_DATE":"2017-12-22T00:00:00",
      // "TRAY_HARVEST_STATUS":1,
      //"PLANT_ID":1,
      "LIGHT_MIN"		: 1,
      "LIGHT_MAX"		: 1,
      "HUMI_MIN"		: 1,
      "HUMI_MAX"		: 1,
	  "TEMP_MIN"		: 1,
	  "TEMP_MAX"		: 1,
	  "PH_MIN"			: 1,
	  "PH_MAX"			: 1,
	  "EC_MIN"			: 1,
	  "EC_MAX"			: 1,
      "AUTO_STATUS"		: 1,
	  "INTERVAL_UPDATE"	: int(second),
	  "SCHELDULE_SPRAY"	: "21:00:00_21:10:00",
	  "SCHELDULE_LIGHT"	: "21:00:00_21:10:00, ..." //ThờiGian-bật_Số-phút-bật
   }

- HUB nhận được sẽ chuyển về NODE
	{
		"HUB_ID" : "<string>",
		"DEST" : "<string>",
		"CMD_T" : LIBS_GARDEN_NODE,

		"L1" : 12323,
		"L2" : 1223,
		"H1" : 12323,
		"H2" : 12,
		"T1" : 123,
		"T2" : 12,
		"P1" : 12,
		"P2" : 331,
		"E1" : 3231,
		"E2" : 1232,
		"AS" : 2323231,
		"IU" : 233232322,
		"SD" : "21:00:00_21:10:00, ..." //ThờiGian-bật_Số-phút-bật
	}

#end region

#region HUB STATUS
- Topic thông báo trạng thái của HUB "AGRISYSTEM/<HubID>/STATUS"
  + Khi HUB kết nối đến MQTT BROKER
	{
		"HUB_ID" : "<HubID>",
		"STATUS" : "ONLINE"
	}
  + Khi HUB mất kết nối đển MQTT BROKER, BROKER sẽ tự động publish gói tin LWT với nội dung:
	{
		"HUB_ID" : "<HubID>",
		"STATUS" : "OFFLINE"
	}
Các gói tin gửi vào đây sẽ được RETAIN.
#end region

#region ACTION LOGS
Topic: "AGRISYSTEM/<HubID>/LOGS"
	{
		"MES_ID"		: "",
		"HUB_ID"		: "<HubID>",
		"SOURCE"		: "<DeviceID>",
		"DEST"			: "SERVER",
		"TIMESTAMP"		: "155555555555555",
		"CMD_T"			: UPDATE_ACTION_LOGS,
		"ACTION_NAME"	: "PUMP ON",
		"ACTION_FROM"	: "HIC0001",
		"ACTION_TO"		: "N13123"
	}
#end region


#region NOTIFICATION
HUB -> SERVER
Topic : "AGRISYSTEM/<HubID>/NOTIFY/HUB"
	{
		"MES_ID"		: "",
		"HUB_ID"		: "<HubID>",
		"SOURCE"		: "<DeviceID>",
		"DEST"			: "SERVER",
		"TIMESTAMP"		: "155555555555555", //DEVICE gửi lên có thể ko cần trường này, HUB sẽ tự thêm
		"CMD_T":		: COMMAND_TYPE::NOTIFICATION,
		"NOTI_T"		: NOTIFICATION_TYPE
	}

APP -> HUB
Topic: "AGRISYSTEM/<HubID>/NOTIFY/APP"
	{
		"MES_ID"		: "",
		"HUB_ID"		: "<HubID>",
		"SOURCE"		: "SERVER",
		"DEST"			: "<HubID>",
		"TIMESTAMP"		: "155555555555555", //DEVICE gửi lên có thể ko cần trường này, HUB sẽ tự thêm
		"CMD_T"			: COMMAND_TYPE::ADD_NEW_TRAY
	}
#end region


#endif