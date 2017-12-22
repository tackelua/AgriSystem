#define IGNORE_THIS_FILE
#ifndef IGNORE_THIS_FILE
/*
--------------------------------------------------------------------------------------------------------
Thêm 3 trường MES_ID, SOURCE, DEST vào mỗi gói tín, sau này dễ dàng trong việc mở rộng theo nhiều hướng.
--------------------------------------------------------------------------------------------------------
 */

#region HUB & SERVER MQTT

enum COMMAND_TYPE {
	S2H_CONTROL_RELAY = 0,
	S2H_GET_HUB_STATUS,
	S2H_GET_SENSOR_DATA,

	H2S_UPDATE_HUB_STATUS,
	H2S_UPDATE_NODE_DATA
};
enum NODE_TYPE {
	HUB_GATEWAY = 0,
	SOIL_MOISTURE,
	ENVIROMENT_STATION
};

/*
	HUB
		LIGHT ----- Đèn
			ON	
			OFF
		FAN ------- Quạt thông gió
			ON
			OFF
		SPRAY ------ Bơm phun sương
			ON
			OFF
		COVER ----- Mái che
			ON
			OFF
			STOP --	Request
			MID --- Response
		
	SOIL_MOISTURE aka TRAY
		MANURE ---- Bơm bón phân
			ON
			OFF
		SPRAY ----- Bơm tưới nước
			ON
			OFF
		LIGHT ----- Đèn chiếu sáng
			ON
			OFF
		LED_MOSFET  Đèn quang hợp, có thể điều chỉnh độ sáng
			PC<PERCENT>: PC45, PC78
*/

- Server gửi lệnh điều khiển cho Hub
Topic: "AGRISYSTEM/<HubID>"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"CMD_T"		 : S2H_CONTROL_RELAY,
		"LIGHT"		 : "ON",
		"FAN"		 : "ON",
		"SPRAY"		 : "ON",
		"COVER"		 : "OFF" //sẽ có 3 giá trị cho cái mái che này: ON, OFF, STOP. ON = OPEN, OFF = CLOSE
	}

Topic: "AGRISYSTEM/<HubID>"
	Hub Response:
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"CMD_T"		 : H2S_UPDATE_HUB_STATUS,
		"LIGHT"		 : "ON",
		"FAN"		 : "ON",
		"SPRAY"		 : "ON",
		"COVER"		 : "OFF"	// ON, OFF or MID. MID là ở giữa, ko đụng đến công tắc hành trình ở biên
	}
	
Topic: "AGRISYSTEM/<HubID>"
- Lấy trạng thái của HUB
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"CMD_T"		 : S2H_GET_HUB_STATUS,
	}

Topic: "AGRISYSTEM/<HubID>"
	Hub Response:
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"CMD_T"		 : H2S_UPDATE_HUB_STATUS,
		"LIGHT"		 : "ON",
		"FAN"		 : "ON",
		"SPRAY"		 : "ON",
		"COVER"		 : "OFF"
	}

- Server gửi lệnh điều khiển cho Node
Topic: "AGRISYSTEM/<HubID>"
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"CMD_T"		 : S2H_CONTROL_RELAY,
		"MANURE"	 : "ON",
		"SPRAY"		 : "ON",
		"LIGHT"		 : "OFF",
		"LED_MOSFET" : "PC45"
	}

Topic: "AGRISYSTEM/<HubID>/<NodeID>"
	Hub Response:
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"CMD_T"		 : H2S_UPDATE_NODE_STATUS,
		"MANURE"	 : "ON",
		"SPRAY"		 : "ON",
		"LIGHT"		 : "OFF",
		"LED_MOSFET" : "PC45"
	}

Topic: "AGRISYSTEM/<HubID>"
- Lấy thông tin Sensor
	{
		"MES_ID"	 : "",
		"HUB_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"CMD_T"		 : S2H_GET_SENSOR_DATA
	}
	
Topic: "AGRISYSTEM/<HubID>/<NodeID>"
	Hub Response:
	{
		"MES_ID"	 : "",
		"SOURCE"	 : "",
		"DEST"		 : "",
		"CMD_T"		 : H2S_UPDATE_NODE_DATA,
		"NODE_T"	 : node_type, //Phần data của node phụ thuộc vào "node_type"
			           
		"TEMP"		 : temp,
		"HUMI"		 : humi,
			           
		"MANURE"	 : "ON",
		"SPRAY"		 : "ON",
		"LIGHT"		 : "OFF",
		"LED_MOSFET" : "PC45"
	}

#endregion


#region LIBRARIES
- HUB sẽ subscribe vào topic "AGRISYSTEM/<HubID>/LIBS/#"
- Thông tin về thư viện của từng TRAY sẽ được APP or SERVER publish lên topic "AGRISYSTEM/<HubID>/LIBS/<TrayID>" with retain
Nội dung tùy 
   {  
      "TRAY_ID":"ABC",
      "HUB_CODE":"AB10027",
      // "TRAY_NAME":"Cải",
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
      "AUTO_STATUS":1
   }
#end region

#region HUB & CLIENT NODE
Truyền qua RF hoặc RS485 đôi khi dữ liệu không được toàn vẹn, do đó trong chuỗi gửi đi kèm theo trường GCS để kiểm tra sự toàn vẹn của dữ liệu nhận được.

enum COMMAND_TYPE {
	N2H_REGISTER_NEW_NODE,
	H2N_INFO_RESPONSE,
	N2H_DATA_FROM_SENSORS,
	H2N_GET_DATA
}

- Hub gửi request đến CM để get data
	{
		"MES_ID" : "",
		"HUB_ID" : "",
		"SOURCE" : "",
		"DEST"	 : "",
		"CMD_T"  : H2N_GET_DATA,
		"GCS"	 : (unsigned long)				//Gith checksum
	}

- Gửi Data sensor từ Client Module 
	{
		"MES_ID" : "",
		"HUB_ID" : "",
		"SOURCE" : "",
		"DEST"	 : "",
		"CMD_T"  : N2H_DATA_FROM_SENSORS,
		"TEMP"   : "",						//temp
		"HUMI"   : "",						//humidity
		"RL_STT" : "ON",					//relay status
		"GCS"	 :  (unsigned long)			//Gith checksum
	}

#endregion

#endif