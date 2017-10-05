

void radio_init() {
	radio_received.reserve(200);
	radio.begin();
	radio.setAutoAck(true);
	radio.setRetries(1, 5);
	radio.setDataRate(RF24_1MBPS);    // T?c ?? truy?n
	radio.setPALevel(RF24_PA_MAX);      // Dung l??ng t?i ?a
	radio.setChannel(14);               // ??t k�nh
	radio.setPayloadSize(32);
	radio.openReadingPipe(1, RF_PIPE);
	radio.startListening();
	Db("channel: ");
	DB(radio.getChannel());
	DB("Started");
}

/*
	radio_received
*/
bool radio_available() {
	static unsigned long _timeout = 50;
	radio_received = "";
	char buffer[33];
	if (radio.available()) {
		while (radio.available()) {
			radio.read(buffer, 32);
			buffer[32] = '\0';
			radio_received += buffer;
			for (byte i = 0; i < _timeout; i++) {
				if (!radio.available()) {
					delay(1);
				}
				else {
					continue;
				}
			}
		}
		return true;
	}

	return false;
}

void radio_send(String data) {
	String buffer = data;
	DB(F("#RADIO SENT:"));
	DB(buffer);
	//int segment_i = 0;
	radio.stopListening();
	radio.openWritingPipe(RF_PIPE);
	while (buffer.length() > 32) {
		String segment = buffer.substring(0, 32);
		buffer = buffer.substring(32);
		//Db("Segment");
		//Db(segment_i++);
		//Db("\t[32] ");
		//DB(segment);
		radio.write(segment.c_str(), 32);
	}
	if ((0 < buffer.length()) && (buffer.length() <= 32)) {
		//Db("Segment");
		//Db(segment_i++);
		//Db("\t[");
		//Db(buffer.length());
		//Db("] ");
		//DB(buffer);
		radio.write(buffer.c_str(), buffer.length());
	}
	DB("#SENT");
	radio.openReadingPipe(1, RF_PIPE);
	radio.startListening();

}