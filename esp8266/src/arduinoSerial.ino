#include "esp8266.h"

void readSerial ()
{
	long result;
	int infoMode, infoType, errorType;

	// If nothing is comming, we stop here
	if (!Serial.available())
		return;

	printlnNoPrefix();
	decodeRequest (Serial.readStringUntil ('z'), &result, &infoMode, &infoType, &errorType);

	if (infoType == TYPE_RTM)
		sendTime();  // We send the time to the Arduino
	else if (infoType == TYPE_RIF)
		println ("Info request needs to be sent by web client");
} // readSerial

void initSerial ()
{
	Serial.begin (BAUD_RATE);
	delay (10);
}
