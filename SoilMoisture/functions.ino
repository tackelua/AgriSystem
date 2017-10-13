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
			jsRegister[node_id_] = NodeID;

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

		if (ct == H2N_GET_DATA) {
			thisNode.humi = sensor.readHumidity();
			thisNode.temp = sensor.readTemperatureC();
			node_send_data();
		}
	}
}

/*test sensor*/

void test_sensor() {
	thisNode.humi = sensor.readHumidity();
	thisNode.temp = sensor.readTemperatureC();
	node_send_data();
	delay(5000);
}

void node_send_data()
{
	DynamicJsonBuffer jsBuffer(200);
	JsonObject& jsData = jsBuffer.createObject();

	jsData[CMD_T] = int(N2H_DATA_FROM_SENSORS);
	jsData[HID] = HubID;
	jsData[NID] = NodeID;
	jsData[TEMP] = String(thisNode.temp, 2);
	jsData[HUMI] = String(thisNode.humi, 2);
	jsData[RL_STT] = thisNode.relay == RL_ON ? "ON" : "OFF";
	jsData[GCS] = "";

	String js_send;
	jsData.printTo(js_send);
	radio_send(js_send);
}