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

#pragma region CONTROL HARDWARE
void controlHubRelay(int pin, String status, bool publish = false)){

}
#pragma endregion


