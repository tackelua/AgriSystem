void com_callback_button(button_t button) {

}

void com_callback_mqtt_send(String data) {

}

void CORE_SERIAL_handle() {
	if (CORE_SERIAL.available()) {
		core_serial_received = CORE_SERIAL.readString();
		DB(core_serial_received);
	}
}