// 
// 
// 

#include "hardware.h"

#ifdef ARDUINO_AVR_NANO
bool relayStt_1;
bool relayStt_2;
bool relayStt_3;
bool relayStt_4;
#endif // ARDUINO_AVR_NANO

extern void pinOut(byte relay, bool status);;

void hardware_init() {
#ifdef ARDUINO_AVR_NANO
	pinMode(RELAY_1, OUTPUT);
	pinMode(RELAY_2, OUTPUT);
	pinMode(RELAY_3, OUTPUT);
	pinMode(RELAY_4, OUTPUT);
	pinOut(RELAY_1, RL_OFF);
	pinOut(RELAY_2, RL_OFF);
	pinOut(RELAY_3, RL_OFF);
	pinOut(RELAY_4, RL_OFF);

	pinMode(CONFIG_SEN, INPUT);
	pinMode(BUTTONS, INPUT);
	pinMode(LCD_LIGHT, OUTPUT);

#endif // ARDUINO_AVR_NANO
}
