// I2CMaster.h
#include <Wire.h>

#ifndef _I2CMASTER_h
#define _I2CMASTER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define WATERLEVEL_SENSOR		0x39

typedef enum Command_ID
{
	FreeCommand = 0,
	ReadSensor,
	SetScanInterval
};

typedef enum Sensor_ID
{
	NoSensor = 0,
	LightSensor, TempCTDS, TDSSensor, PHSensor,
	HumidityAir, TempCAir, TempFAir,
	HumiditySoil, TempCSoil, TempFSoil,
	WaterLevel, WaterDetect
};

class I2CMASTER
{
protected:

private:
	// Send data to I2C Slave
	void send(byte slave_id, byte *dat, byte len);

	// Request data from I2C Slave
	bool request(byte slave_id, byte *dat, byte len);

public:
	// Initial I2C Hardware
	// TODO: Must add this function before run another I2C code
	void init();

	// Read data from Sensor Control MCU
	// - slave_id: I2C Slave Address
	// - sensor_id: the id of sensor, sensor id listed at "typedef enum Sensor_ID"
	// - dat: array buffer to save the result
	// - len: lenght of result in byte, it dependent on sensor type
	bool readSensor(byte slave_id, byte sensor_id, byte *dat, byte len);

	// Set Sensor Scan Interval in Milisecond for Sensor Control MCU
	// It s also can set power mode for Sensor module
	void setSensorScanInterval(byte slave_id, int scan_interval);
};

#endif

