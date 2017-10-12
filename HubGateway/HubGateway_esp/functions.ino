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
		DynamicJsonBuffer jsBuffer(200);
		JsonObject& jsCoreBuff = jsBuffer.parseObject(core_serial_received);

		int cmd_t = jsCoreBuff[CMD_T].as<String>().toInt();
		if (cmd_t == N2H_DATA_FROM_SENSORS)
		{
			float t = jsCoreBuff[TEMP].as<String>().toFloat();
			float h = jsCoreBuff[HUMI].as<String>().toFloat();
			lcd_show_sensor_data(t, h);
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
