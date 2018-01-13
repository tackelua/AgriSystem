#define IGNORE_THIS_FILE
#ifndef IGNORE_THIS_FILE

#define API_VERSION  "0.1.7"

/*
	v0.1.7
		Quy định retain cho các COMMAND_TYPE
	v0.1.6
		Quy định cách đặt ID cho từng thiết bị
	v0.1.5
		Fix LIBRARIES
	v0.1.4
		Add Schedule ON, OFF
*/

Cách đặt ID cho các thiết bị:
	GARDEN_HUB		   : HXXXXX
	GARDEN_NODE		   : NXXXXX
	ENVIROMENT_MONITOR : EXXXXX
	TANK_CONTROLER	   : TXXXXX

Trong đó kí tự đầu tiên đại diện cho loại thiết bị, các kí tự còn lại mã xác định ID.

/*
--------------------------------------------------------------------------------------------------------
Thêm 3 trường MES_ID, SOURCE, DEST vào mỗi gói tín, sau này dễ dàng trong việc mở rộng theo nhiều hướng.
--------------------------------------------------------------------------------------------------------
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

	LIBS_GARDEN_NODE							//with retain
};

enum NODE_TYPE {
	GARDEN_HUB = 0,
	GARDEN_NODE,
	ENVIROMENT_MONITOR,
	TANK_CONTROLER
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
	Topic: "AGRISYSTEM/<HubID>"
	{
		"MES_ID"	 : "<string>",
		"HUB_ID"	 : "<string>",
		"SOURCE"	 : "<string>",
		"DEST"		 : "<string>",
		"CMD_T"		 : CONTROL_GARDEN_HUB,
		"LIGHT"		 : "ON",
		"FAN"		 : "ON",
		"SPRAY"		 : "ON",
		"COVER"		 : "OFF" //sẽ có 3 giá trị cho cái mái che này: ON, OFF, STOP. ON = OPEN, OFF = CLOSE
	}
		
GET_DATA_GARDEN_HUB
	Topic: "AGRISYSTEM/<HubID>"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"CMD_T"		 : GET_DATA_GARDEN_HUB
	}
		
UPDATE_DATA_GARDEN_HUB
	Topic: "AGRISYSTEM/<HubID>"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"CMD_T"		 : UPDATE_DATA_GARDEN_HUB,
		"LIGHT"		 : "ON",
		"FAN"		 : "ON",
		"SPRAY"		 : "ON",
		"COVER"		 : "OFF"	// ON, OFF or MID. MID là ở giữa, ko đụng đến công tắc hành trình ở biên
	}
#end region

#region GARDEN_NODE
CONTROL_GARDEN_NODE
	Topic: "AGRISYSTEM/<HubID>"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"CMD_T"		 : CONTROL_GARDEN_NODE,
		"MANURE"	 : "ON",
		"SPRAY"		 : "ON",
		"LIGHT"		 : "OFF",
		"LED_MOSFET" : "PC45"
	}
GET_DATA_GARDEN_NODE
	Topic: "AGRISYSTEM/<HubID>"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"CMD_T"		 : GET_DATA_GARDEN_NODE
	}

UPDATE_DATA_GARDEN_NODE
	Topic: "AGRISYSTEM/<HubID>/<NodeID>"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"CMD_T"		 : UPDATE_DATA_GARDEN_NODE,
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
#end region

#region ENVIROMENT_MONITOR
CONTROL_ENVIROMENT_MONITOR
	none
	
GET_DATA_ENVIROMENT_MONITOR
	Topic: "AGRISYSTEM/<HubID>"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "<ENVIROMENT_ID>",
		"CMD_T"		 : GET_DATA_ENVIROMENT_MONITOR
	}
UPDATE_DATA_ENVIROMENT_MONITOR
	Topic: "AGRISYSTEM/<HubID>/<ENVIROMENT_ID>"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
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
	Topic: "AGRISYSTEM/<HubID>"
	{
		"MES_ID"	 : "<string>",
		"HUB_ID"	 : "<string>",
		"SOURCE"	 : "<string>",
		"DEST"		 : "<string>",
		"CMD_T"		 : CONTROL_TANK_CONTROLER,
		"WATER_IN"	 : "ON",
		"WATER_OUT"	 : "OFF"
	}
GET_DATA_TANK_CONTROLER
	Topic: "AGRISYSTEM/<HubID>"
	{
		"MES_ID"	 : "<string>",
		"HUB_ID"	 : "<string>",
		"SOURCE"	 : "<string>",
		"DEST"		 : "<string>",
		"CMD_T"		 : GET_DATA_TANK_CONTROLER
	}

UPDATE_DATA_TANK_CONTROLER
	Topic: "AGRISYSTEM/<HubID>/<TankControlerID>"
	{
		"MES_ID"	 : "<string>",
		"HUB_ID"	 : "<string>",
		"SOURCE"	 : "<string>",
		"DEST"		 : "<string>",
		"CMD_T"		 : UPDATE_DATA_TANK_CONTROLER,
		
		"WATER_IN"	 : "ON",
		"WATER_OUT"	 : "OFF",
		
		"WATER_LEVEL": "<PERCENT>",
		"WATER_HIGH" : "YES",
		"WATER_LOW"	 : "NO"
	}
#end region


#region LIBRARIES
- HUB sẽ subscribe vào topic "AGRISYSTEM/<HubID>/LIBS/#"
- Thông tin về thư viện của từng TRAY sẽ được APP or SERVER publish lên topic "AGRISYSTEM/<HubID>/LIBS/<TrayID>" with retain
Nội dung tùy 
   {
	  "MES_ID"	 : "<string>",
	  "HUB_ID": "<string>",
	  "SOURCE" : "<string>",
	  "DEST" : "<string>",
	  "CMD_T" : LIBS_GARDEN_NODE,

      "TRAY_NAME":"Cải", //bỏ dấu please
      // "TRAY_IMAGE":"https://i.imgur.com/zLHjhzW.jpg",
      // "TRAY_CREATED_DATE":"2017-12-22T00:00:00",
      // "TRAY_HARVEST_STATUS":1,
      //"PLANT_ID":1,
      "LIGHT_MIN":1,
      "LIGHT_MAX":1,
      "HUMI_MIN":1,
      "HUMI_MAX":1,
      "TEMP_MIN":1,
      "TEMP_MAX":1,
      "AUTO_STATUS":1,
	  "INTERVAL_UPDATE": int(second),
	  "SCHELDULE" : "21:00:00_21:10:00, ..." //Giờ bật_tắt
   }
#end region



#endif