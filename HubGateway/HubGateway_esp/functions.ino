void com_callback_button(button_t button) {

}

void com_callback_mqtt_send(String data) {

}

void CORE_SERIAL_handle() {
	if (CORE_SERIAL.available()) {
		digitalWrite(LED_STATUS, LOW);
		core_serial_received = CORE_SERIAL.readString();
		digitalWrite(LED_STATUS, HIGH);
		DB(core_serial_received);

		mqtt_publish(mqtt_common_topic, core_serial_received);

		DynamicJsonBuffer jsBuffer(200);
		JsonObject& jsCoreBuff = jsBuffer.parseObject(core_serial_received);

		int cmd_t = jsCoreBuff[CMD_T].as<String>().toInt();
		if (cmd_t == N2H_DATA_FROM_SENSORS)
		{
			float t = jsCoreBuff[TEMP].as<String>().toFloat();
			float h = jsCoreBuff[HUMI].as<String>().toFloat();
			if (t > 0.0 && h > 0.0)
			{
				lcd_show_sensor_data(t, h);
				ThingSpeak_uploadData(t, h);
			}
		}
		else if (cmd_t == CC_SEND_BUTTON_PRESS)
		{
			int btn = jsCoreBuff[DATA].as<int>();
			Db(F("Button "));
			DB(btn);
			
		}
	}
}

void lcd_show_sensor_data(float temp, float humi)
{
	display.setCursor(30, 17);
	for (int i = 0; i < 9; i++) //xóa dữ liệu cũ, print nó không tự xóa
	{
		display.drawChar(30 + i * 3, 17, ' ', display_contrast_level, 0, 1);
	}
	display.setCursor(30, 33);
	for (int i = 0; i < 9; i++)
	{
		display.drawChar(30 + i * 3, 33, ' ', display_contrast_level, 0, 1);
	}
	display.display();

	display.setCursor(30, 17);
	display.print(temp);
	display.setCursor(30, 33);
	display.print(humi);
	display.display();
}


void updateFirmware(String url) {
	ESPhttpUpdate.rebootOnUpdate(true);

	t_httpUpdate_return ret = ESPhttpUpdate.update(url);

	switch (ret) {
	case HTTP_UPDATE_FAILED:
		DEBUG.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
		break;

	case HTTP_UPDATE_NO_UPDATES:
		DEBUG.println(F("HTTP_UPDATE_NO_UPDATES"));
		break;

	case HTTP_UPDATE_OK:
		DEBUG.println(F("HTTP_UPDATE_OK"));
		delay(2000);
		ESP.restart();
		break;
	}
}

#pragma region Thing Speak

// ThingSpeak Settings
const int ThingSpeak_channelID = 348754; //
String ThingSpeak_writeAPIKey = "7ZHWI44QXUNGG75S"; // write API key for your ThingSpeak Channel
const char* ThingSpeak_server = "api.thingspeak.com";

WiFiClient WFclient;

void ThingSpeak_uploadData(float temp, float humi)
{
	DB(F("ThingSpeak"));
	if (WFclient.connect(ThingSpeak_server, 80)) {
		// Construct API request body
		String body = "field1=" + String(temp, 1) + "&field2=" + String(humi, 1);

		WFclient.print("POST /update HTTP/1.1\n");
		WFclient.print("Host: api.thingspeak.com\n");
		WFclient.print("Connection: close\n");
		WFclient.print("X-THINGSPEAKAPIKEY: " + ThingSpeak_writeAPIKey + "\n");
		WFclient.print("Content-Type: application/x-www-form-urlencoded\n");
		WFclient.print("Content-Length: ");
		WFclient.print(body.length());
		WFclient.print("\n\n");
		WFclient.print(body);
		WFclient.print("\n\n");
		Db("TEMP: " + String(temp, 1) + "\r\nHUMI: " + String(humi, 1));
	}
	WFclient.stop();
}
#pragma endregion

#pragma region WiFi Init
void wifi_init() {
	//WiFi.begin("IoT Wifi", "mic@dtu12345678()");

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


