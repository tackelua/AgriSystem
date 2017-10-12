// 
// 
// 

#include "hardware.h"

SHT1x sensor(SHT_DATA, SHT_CLK);

void hardware_init()
{
	pinMode(LED_STATUS, OUTPUT);
	pinMode(LED_RELAY, OUTPUT);
	pinMode(RELAY, OUTPUT);
	pinMode(CONFIG_SEN, INPUT);

	digitalWrite(LED_STATUS, LED_OFF);
	digitalWrite(LED_RELAY, LED_OFF);
	digitalWrite(RELAY, RL_OFF);

	/*test pin*/
	//DB(F("Test LED_STATUS"));
	//for (byte i = 0; i < 5; i++) {
	//	digitalWrite(LED_STATUS, HIGH);
	//	delay(500);
	//	digitalWrite(LED_STATUS, LOW);
	//	delay(500);
	//}
	//
	//DB(F("Test LED_RELAY"));
	//for (byte i = 0; i < 5; i++) {
	//	digitalWrite(LED_RELAY, HIGH);
	//	delay(500);
	//	digitalWrite(LED_RELAY, LOW);
	//	delay(500);
	//}
	//
	//DB(F("Test RELAY"));
	//for (byte i = 0; i < 5; i++) {
	//	digitalWrite(RELAY, HIGH);
	//	delay(500);
	//	digitalWrite(RELAY, LOW);
	//	delay(500);
	//}
}
