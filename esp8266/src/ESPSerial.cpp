#include <Time.h>
#include "ESPSerial.h"
#include "Logger.h"
#include "Request.h"
// #include "VariableChange.h"
// #include "Alarms.h"
#include "Utils.h"
#include "TimeLib.h"
// #include "Wifi.h"

void ESPSerial::init (long serialBaudRate)
{
	Serial.begin (serialBaudRate); // Initialize debug communication
}

// Receive datas from ESP8266 for Wi-Wi control
void ESPSerial::receiveAndDecode ()
{
	if (!Serial.available())
		return;

	const int bufSize = 14;
	char buf[bufSize];
	uint8_t type = TYPE_UNK;
	uint8_t complement;
	int32_t information;
	int8_t error;

	uint8_t length;

	if (Serial.available())
		length = Serial.readBytesUntil ('z', buf, bufSize);
	else if (Serial1.available())
		length = Serial1.readBytesUntil ('z', buf, bufSize);

	buf[length] = '\0';

	request.decode (buf, type, complement, information, error);

	if (type == TYPE_RTM)
		sendTime();  // We send the time to the Arduino
	else if (type == TYPE_RIF)
		Log.trace ("Info request needs to be sent by web client");
}

void ESPSerial::sendTime ()
{
	char buf[30];

	// wifi.getTime();

	Log.trace ("Time is: %s" dendl, utils.clock (buf));

	if (timeStatus() != timeNotSet)
	{
		Log.trace ("Sending time to arduino: ");

		Serial.print ("TIM");
		Serial.print (now());
		Serial.print ('z');

		Log.tracenp (dendl);
	}
	else
	{
		Log.warning ("Time not set, not sending time to arduino..." dendl);
	}
}

ESPSerial serial = ESPSerial();
