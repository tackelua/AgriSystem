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
