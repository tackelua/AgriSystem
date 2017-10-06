#define Githkey		"Gith"
#define CMD_T		"CMD_T"
#define NODE_T		"NODE_T"
#define NID			"NID"
#define TEMP		"TEMP"
#define HUMI		"HUMI"
#define RL_STT		"RL_STT"
#define GCS			"GCS"
#define RF_ADDR		"RF_ADDR"
#define RF_CHN		"RF_CHN"
#define ON			"ON"
#define OFF			"OFF"
#define HID			"HID"
#define R1			"R1"
#define R2			"R2"
#define R3			"R3"
#define R4			"R4"

void esp_send(esp_command_code ECC, String data);
void esp_send(esp_command_code ECC, String data) {
	ESP_Serial.println(F(">"));
	ESP_Serial.println(ECC);
	ESP_Serial.println(data);
	ESP_Serial.println();
}

void H2N_getData(String node_id) {
	DynamicJsonBuffer jsBuffer(300);
	JsonObject& js2RF = jsBuffer.createObject();
	js2RF[CMD_T] = H2N_GET_DATA;
	js2RF[NID] = node_id;
	js2RF[GCS] = gcs_calc(String(H2N_GET_DATA) + node_id + Githkey);

	String js_sent;
	js2RF.printTo(js_sent);
	radio_send(js_sent);
}

void H2N_infoResponse() {
	DynamicJsonBuffer jsBuffer(300);
	JsonObject& js2RF = jsBuffer.createObject();
	js2RF[CMD_T] = H2N_INFO_RESPONSE;
	String rf_addr = String(uint32_t(RF_PIPE >> 32)) + String((uint32_t)RF_PIPE);
	js2RF[RF_ADDR] = rf_addr;
	js2RF[RF_CHN] = RF_CHANNEL;
	String gcsHashString = String(byte(H2N_INFO_RESPONSE)) + rf_addr + String(RF_CHANNEL) + String(Githkey);
	String gcs = gcs_calc(gcsHashString);
	js2RF[GCS] = gcs;

	String js_sent;
	js2RF.printTo(js_sent);
	radio_send(js_sent);
}
void H2S_updateHubStatus() {
	String RL_STT_1 = (relayStt_1 == RL_ON ? ON : OFF);
	String RL_STT_2 = (relayStt_2 == RL_ON ? ON : OFF);
	String RL_STT_3 = (relayStt_3 == RL_ON ? ON : OFF);
	String RL_STT_4 = (relayStt_4 == RL_ON ? ON : OFF);
	DynamicJsonBuffer jsBuffer(300);
	JsonObject& js2RF = jsBuffer.createObject();
	js2RF[CMD_T] = H2S_UPDATE_HUB_STATUS;
	js2RF[HID] = HUB_ID;
	js2RF[R1] = RL_STT_1;
	js2RF[R2] = RL_STT_2;
	js2RF[R3] = RL_STT_3;
	js2RF[R4] = RL_STT_4;
	js2RF[GCS] = gcs_calc(String(H2S_UPDATE_HUB_STATUS) + RL_STT_1 + RL_STT_2 + RL_STT_3 + RL_STT_4 + Githkey);

	String js_sent;
	js2RF.printTo(js_sent);
	esp_send(SEND_TO_SERVER, js_sent);
}


void H2S_updateNodeData(node_t node_id);
void H2S_updateNodeData(node_t node_id) {
	DynamicJsonBuffer jsBuffer(300);
	JsonObject& js2RF = jsBuffer.createObject();
	js2RF[CMD_T] = H2S_UPDATE_NODE_DATA;
	js2RF[HID] = HUB_ID;
	js2RF[NID] = node_id.id;
	js2RF[NODE_T] = (int)node_id.type;
	js2RF[RL_STT] = node_id.relay;
	js2RF[GCS] = gcs_calc(String(H2S_UPDATE_NODE_DATA) + HUB_ID + node_id.id + String(node_id.type) + node_id.relay + Githkey);

	String js_sent;
	js2RF.printTo(js_sent);
	esp_send(SEND_TO_SERVER, js_sent);
}

extern String mqtt_received;
void handle_command_server() {
	DynamicJsonBuffer jsBuffer(300);
	JsonObject& command = jsBuffer.parseObject(mqtt_received);
	COMMAND_TYPE cmd_t = (COMMAND_TYPE)command[CMD_T].as<int>();
	switch (cmd_t)
	{
	case S2H_CONTROL_RELAY:
	{
		String r1 = command[R1].as<String>();
		String r2 = command[R2].as<String>();
		String r3 = command[R3].as<String>();
		String r4 = command[R4].as<String>();
		hub_control_relay(r1, r2, r3, r4);
		H2S_updateHubStatus();
		break;
	}
	case S2H_GET_HUB_STATUS:
	{
		H2S_updateHubStatus();
		break;
	}

	case S2H_GET_SENSOR_DATA:
	{
		String nid = command[NID].as<String>();
		H2N_getData(nid);
		break;
	}

	default:
		break;
	}
}

extern String radio_received;
void handle_radio_received() {
	DynamicJsonBuffer jsBuffer(300);
	JsonObject& command = jsBuffer.parseObject(radio_received);
	COMMAND_TYPE cmd_t = (COMMAND_TYPE)command[CMD_T].as<int>();
	switch (cmd_t)
	{
	case N2H_REGISTER_NEW_NODE:
	{
		NODE_TYPE nodeType = (NODE_TYPE)command[NODE_T].as<int>();
		String nid = command[NID].as<String>();
		String gcs = command[GCS].as<String>();

		break;
	}
	case N2H_DATA_FROM_SENSORS:
	{
		node_t sensor;
		sensor.id = command[NID].as<String>();
		sensor.temp = command[TEMP].as<float>();
		sensor.humi = command[HUMI].as<float>();
		sensor.relay = command[RL_STT].as<String>();
		String gcs = command[GCS].as<String>(); 
		H2S_updateNodeData(sensor);
		break;
	}
	default:
		break;
	}
}
