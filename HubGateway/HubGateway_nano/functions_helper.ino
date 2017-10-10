void pinOut(byte relay, bool status) {
	switch (relay) {
	case RELAY_1:
		relayStt_1 = status;
		digitalWrite(RELAY_1, relayStt_1);
		break;
	case RELAY_2:
		relayStt_2 = status;
		digitalWrite(RELAY_2, relayStt_2);
		break;
	case RELAY_3:
		relayStt_3 = status;
		digitalWrite(RELAY_3, relayStt_3);
		break;
	case RELAY_4:
		relayStt_4 = status;
		digitalWrite(RELAY_4, relayStt_4);
		break;
	default:
		break;
	}
}

void hub_control_relay(String r1, String r2, String r3, String r4) {
	if (r1 == ON) {
		pinOut(RELAY_1, RL_ON);
	}
	else if (r1 == OFF) {
		pinOut(RELAY_1, RL_OFF);
	}

	if (r2 == ON) {
		pinOut(RELAY_2, RL_ON);
	}
	else if (r2 == OFF) {
		pinOut(RELAY_2, RL_OFF);
	}

	if (r3 == ON) {
		pinOut(RELAY_3, RL_ON);
	}
	else if (r3 == OFF) {
		pinOut(RELAY_3, RL_OFF);
	}

	if (r4 == ON) {
		pinOut(RELAY_4, RL_ON);
	}
	else if (r4 == OFF) {
		pinOut(RELAY_4, RL_OFF);
	}
}

int readButtons() {
	static const unsigned long debound_time = 20;
	static unsigned long t_lastChange;
	int val = analogRead(BUTTONS);
	static button_t but_last = NO_BUTTON;
	static button_t but_now;
	/*
		LEFT == 166;
		DOWN == 200;
		UP == 251;
		RIGHT == 335;
		BACK == 511;
		ENTER == 1021;
	*/
	if (100 < val && val < 183) {
		but_now = LEFT;
	}
	else if (183 < val && val < 225) {
		but_now = DOWN;
	}
	else if (225 < val && val < 293) {
		but_now = UP;
	}
	else if (293 < val && val < 423) {
		but_now = RIGHT;
	}
	else if (423 < val && val < 766) {
		but_now = BACK;
	}
	else if (766 < val) {
		but_now = ENTER;
	}
	else {
		but_now = NO_BUTTON;
	}

	if (but_now != but_last) {
		t_lastChange = millis();
		but_last = but_now;
	}
	else if ((millis() - t_lastChange) > debound_time) {
		return but_now;
	}
	return NO_BUTTON;
}

