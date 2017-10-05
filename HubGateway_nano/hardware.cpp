// 
// 
// 

#include "hardware.h"

bool relayStt_1;
bool relayStt_2;
bool relayStt_3;
bool relayStt_4;

void hardware_init() {
	pinMode(RELAY_1, OUTPUT);
	pinMode(RELAY_2, OUTPUT);
	pinMode(RELAY_3, OUTPUT);
	pinMode(RELAY_4, OUTPUT);
	pinOut(RELAY_1, RL_OFF);
	pinOut(RELAY_2, RL_OFF);
	pinOut(RELAY_3, RL_OFF);
	pinOut(RELAY_4, RL_OFF);

	pinMode(CONFIG_SEN, INPUT);
}
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