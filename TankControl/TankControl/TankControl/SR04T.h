// SR04T.h

#ifndef _SR04T_h
#define _SR04T_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class SR04T
{
 protected:
	 uint8_t pin_echo, pin_trig;

 public:
	 SR04T(uint8_t trig_pin, uint8_t echo_pin);
	 int read();
};

class IR03
{
protected:
	uint8_t pin_in, pin_out;

public:
	IR03(uint8_t in_pin, uint8_t out_pin);
	bool detect();
};
#endif

