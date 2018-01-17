// 
// 
// 

#include "I2CMaster.h"


void I2CMASTER::init()
{
	Wire.begin();
}

void I2CMASTER::send(byte slave_id, byte * dat, byte len)
{
	Wire.beginTransmission(slave_id);
	for (int i = 0;i < len;i++) Wire.write(dat[i]);
	Wire.endTransmission();
}

bool I2CMASTER::request(byte slave_id, byte * dat, byte len)
{
	bool res = true;
	// Request data from slave.
	Wire.beginTransmission(slave_id);
	int available = Wire.requestFrom(slave_id, (uint8_t)len);

	if (available == len)
	{
		int i = 0;
		while (Wire.available()) dat[i++] = Wire.read();
	}
	else res = false;

	int result = Wire.endTransmission();
	if (result) res = false;
	return res;
}

bool I2CMASTER::readSensor(byte slave_id, byte sensor_id, byte * dat, byte len)
{
	byte tmpCmd[2];
	tmpCmd[0] = ReadSensor;
	tmpCmd[1] = sensor_id;
	send(slave_id, tmpCmd, 2);
	return request(slave_id, dat, len);
}

void I2CMASTER::setSensorScanInterval(byte slave_id, int scan_interval)
{
	byte tmpCmd[3];
	tmpCmd[0] = SetScanInterval;
	tmpCmd[1] = (byte)(scan_interval >> 8);
	tmpCmd[2] = (byte)scan_interval;
 	send(slave_id, tmpCmd, 3);
}
