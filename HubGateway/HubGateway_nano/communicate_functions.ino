

void core_send(core_command_code CCCode, String data);
void core_send(core_command_code CCCode, String data) {
	//CORE_SERIAL.println(F(">"));
	//CORE_SERIAL.println(CCCode);
	//CORE_SERIAL.println(data);
	//CORE_SERIAL.println();
	
	DynamicJsonBuffer jsBuffer(100);
	JsonObject& jsCore = jsBuffer.createObject();
	jsCore[CMD_T] = int(CCCode);
	jsCore[DATA] = data;
	String js_send;
	jsCore.printTo(js_send);
	CORE_SERIAL.print(js_send);
	DB(js_send);

}

void H2N_getData(String node_id) {
	DynamicJsonBuffer jsBuffer(300);
	JsonObject& js2RF = jsBuffer.createObject();
	js2RF[CMD_T] = H2N_GET_DATA;
	js2RF[NID] = node_id;
	js2RF[GCS] = gcs_calc(String(H2N_GET_DATA) + node_id + Githkey);

	String js_send;
	js2RF.printTo(js_send);
	radio_send(js_send);
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

	String js_send;
	js2RF.printTo(js_send);
	radio_send(js_send);
}
void H2S_updateHubStatus() {
	String RL_STT_1 = (relayStt_1 == RL_ON ? ON : OFF);
	String RL_STT_2 = (relayStt_2 == RL_ON ? ON : OFF);
	String RL_STT_3 = (relayStt_3 == RL_ON ? ON : OFF);
	String RL_STT_4 = (relayStt_4 == RL_ON ? ON : OFF);
	DynamicJsonBuffer jsBuffer(300);
	JsonObject& js2RF = jsBuffer.createObject();
	js2RF[CMD_T] = H2S_UPDATE_HUB_STATUS;
	js2RF[HID] = HubID;
	js2RF[R1] = RL_STT_1;
	js2RF[R2] = RL_STT_2;
	js2RF[R3] = RL_STT_3;
	js2RF[R4] = RL_STT_4;
	js2RF[GCS] = gcs_calc(String(H2S_UPDATE_HUB_STATUS) + RL_STT_1 + RL_STT_2 + RL_STT_3 + RL_STT_4 + Githkey);

	String js_send;
	js2RF.printTo(js_send);
	core_send(CC_SEND_TO_SERVER, js_send);
}


void H2S_updateNodeData(node_t node_id);
void H2S_updateNodeData(node_t node_id) {
	DynamicJsonBuffer jsBuffer(300);
	JsonObject& js2RF = jsBuffer.createObject();
	js2RF[CMD_T] = H2S_UPDATE_NODE_DATA;
	js2RF[HID] = HubID;
	js2RF[NID] = node_id.id;
	js2RF[NODE_T] = (int)node_id.type;
	js2RF[RL_STT] = node_id.relay;
	js2RF[GCS] = gcs_calc(String(H2S_UPDATE_NODE_DATA) + HubID + node_id.id + String(node_id.type) + node_id.relay + Githkey);

	String js_send;
	js2RF.printTo(js_send);
	core_send(CC_SEND_TO_SERVER, js_send);
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


//

void transfer_serial_radio() {
	if (Serial.available()) {
		radio_send(Serial.readString());
	}
	if (radio_available()) {
		Serial.println(radio_received);
		CORE_SERIAL.print(radio_received);
	}
}

void transfer_button_status() {
	int btn = readButtons();
	if (btn != NO_BUTTON) {
		DB(btn);
		core_send(CC_SEND_BUTTON_PRESS, String(btn));
	}

	if (btn == BACK)
	{
		digitalWrite(LCD_LIGHT, !digitalRead(LCD_LIGHT));
	}
}