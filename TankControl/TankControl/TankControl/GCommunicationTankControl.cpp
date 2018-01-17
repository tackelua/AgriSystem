// 
// 
// 

#include "GCommunicationTankControl.h"
bool GARDENCOMMUNICATION::getBool(String& cmd, String jkd)
{
	String s = getString(cmd, jkd);
	if (s == "ON") return true;
	return false;
}
int GARDENCOMMUNICATION::getValue(String& cmd, String jkd)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(cmd);
	return root[jkd];
}

String GARDENCOMMUNICATION::getString(String& cmd, String jkd)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(cmd);
	return root[jkd];
}
