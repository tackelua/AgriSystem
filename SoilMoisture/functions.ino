#define node_type_		"nt"
#define node_id_		"nid"
#define command_type_	"ct"
#define rf_addr_		"rfad"
#define rf_channel_		"rfc"
#define temperature_	"temp"
#define humidity_		"humi"

/*Config Serial kiểm tra jack cắm có được kết nối với hub hay ko*/
void check_config_Serial() {
	bool _connected = !digitalRead(CONFIG_SEN);
	static bool flag_cfS_declared = false; //Đánh dấu Serial đã được khai báo
	if (_connected) {
		if (!flag_cfS_declared) {
			flag_cfS_declared = true;
			CONFIG_SERIAL.begin(9600);
			CONFIG_SERIAL.setTimeout(100);
			delay(100);
			CONFIG_SERIAL.readString();	//bỏ buffer

			DynamicJsonBuffer jsBuffer(200);
			JsonObject& jsRegister = jsBuffer.createObject();

			jsRegister[node_type_] = SOIL_MOISTURE;
			jsRegister[node_id_] = NODE_ID;

			String jsExport;
			jsRegister.printTo(jsExport);
			CONFIG_SERIAL.print(jsExport);
		}
		else if (CONFIG_SERIAL.available()) {
			String dataHub = CONFIG_SERIAL.readString();
			dataHub.trim();
			DynamicJsonBuffer jsBuffer(200);
			JsonObject& jsHub = jsBuffer.parseObject(dataHub);
			RF_PIPE = jsHub[rf_addr_].as<String>().toInt();
			RF_CHANNEL = jsHub[rf_channel_].as<String>().toInt();
		}
	}
	else if (flag_cfS_declared) {
		flag_cfS_declared = false;
		~CONFIG_SERIAL;
	}
}

void rf_command_handle() {
	if (radio_available()) {
		DynamicJsonBuffer jsBuffer(200);
		JsonObject& jsHub = jsBuffer.parseObject(radio_received);

		byte ct = jsHub[command_type_].as<byte>();
		
		if(ct == HUB_GET_DATA){
			float _temp;
			float _humi;

			JsonObject& jsData = jsBuffer.createObject();
			jsData[command_type_] = (byte)NODE_SEND_DATA;
			jsData[node_id_] = NODE_ID;
			jsData[temperature_] = _temp;
			jsData[humidity_] = _humi;

			String dataJS;
			jsData.printTo(dataJS);
			radio_send(dataJS);
		}
	}
}
 
/*test sensor*/
void test_sensor() {
	while (true) {
		float h = sensor.readHumidity();
		Db(F("Humi: "));
		DB(h);
		radio_send("Humi: " + String(h));

		float t = sensor.readTemperatureC();
		Db(F("Temp: "));
		DB(t);
		DB();
		radio_send("Temp: " + String(t));
		radio_send("\r\n");
		delay(5000);
	}
}
