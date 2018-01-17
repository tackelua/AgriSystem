// 
// 
// 

#include "SR04T.h"

SR04T::SR04T(uint8_t trig_pin, uint8_t echo_pin)
{
	pin_echo = echo_pin;
	pin_trig = trig_pin;
	pinMode(pin_echo, INPUT);
	pinMode(trig_pin, OUTPUT);
	digitalWrite(pin_echo, HIGH);
}

int SR04T::read()
{
	int distance; unsigned long durationMS = 0;
	delay(1);
	digitalWrite(pin_trig, LOW); // Set the trigger pin to low for 2uS
	delayMicroseconds(4);
	digitalWrite(pin_trig, HIGH); // Send a 10uS high to trigger ranging
	delayMicroseconds(10);
	digitalWrite(pin_trig, LOW); // Send pin low again

	//distance = pulseIn(pin_echo, HIGH, 26000); // Read in times pulse  
	durationMS = pulseIn(pin_echo, HIGH);
	distance = (((int)durationMS * 0.034) / 2); //distance / 58;
	return distance;
}

IR03::IR03(uint8_t in_pin, uint8_t out_pin)
{
	pin_in = in_pin;
	pin_out = out_pin;
	pinMode(in_pin, INPUT);
	pinMode(out_pin, OUTPUT);
}

bool IR03::detect()
{
	bool res = false;
	uint8_t samp = 5;
	while (samp--)
	{
		digitalWrite(pin_out, LOW);
		delay(3);
		bool wd = (digitalRead(pin_in) > 0);
		digitalWrite(pin_out, HIGH);
		if (wd) {
			res = true; break;
		}
		delay(1);
	}
	return res;
}
