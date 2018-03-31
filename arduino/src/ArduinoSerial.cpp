#include <Time.h>
#include "ArduinoSerial.h"
#include "Logger.h"
#include "Request.h"
#include "VariableChange.h"
#include "Alarms.h"

void ArduinoSerial::init (long serialBaudRate)
{
	Serial.begin (serialBaudRate); // Initialize debug communication
}

void ArduinoSerial::init (long serialBaudRate, long serial1BaudRate)
{
	Serial.begin (serialBaudRate);   // Initialize debug communication
	Serial1.begin (serial1BaudRate); // Initialize ESP8266 communication
}

void ArduinoSerial::init (long serialBaudRate, long serial1BaudRate, long serial2BaudRate)
{
	Serial.begin (serialBaudRate);   // Initialize debug communication
	Serial1.begin (serial1BaudRate); // Initialize ESP8266 communication
	Serial2.begin (serial2BaudRate); // Initialize DFPlayer communication
}

void ArduinoSerial::init (long serialBaudRate, long serial1BaudRate, long serial2BaudRate, long serial3BaudRate)
{
	Serial.begin (serialBaudRate);   // Initialize debug communication
	Serial1.begin (serial1BaudRate); // Initialize ESP8266 communication
	Serial2.begin (serial2BaudRate); // Initialize DFPlayer communication
	Serial3.begin (serial3BaudRate); // Initialize DFPlayer communication
}

void ArduinoSerial::waitForTime ()
{
	if (!WAIT_FOR_TIME)
		return;

	askForTime();

	time_t lastMillis = millis();

	while (timeStatus() == timeNotSet) // Doesn't start if time isn't set
	{
		read();

		if (millis() - lastMillis >= 5000)
		{
			if (timeStatus() == timeNotSet)
				Log.verbose ("Time is not set" endl);

			askForTime();
			lastMillis = millis();
		}
	}
	Log.trace ("Time Received" dendl);
} // waitForTime

// Asking for time to the ESP8266 (via internet)
void ArduinoSerial::askForTime ()
{
	Log.verbose ("Kindly asking for time" dendl);
	Serial1.print ("TIMEPLEASEz"); // z is the end character
}

// Receive datas from ESP8266 for Wi-Wi control
void ArduinoSerial::read ()
{
	long result, length;
	int infoMod, infoType, errorType;
	const int bufSize = 14;
	char buf[bufSize] = "";

	if (Serial.available())
	{
		length = Serial.readBytesUntil ('z', buf, bufSize);
	}
	else if (Serial1.available())
	{
		length = Serial1.readBytesUntil ('z', buf, bufSize);
	}
	else
	{
		return;
	}

	buf[length] = '\0';

	request.decode (buf, result, infoMod, infoType, errorType);

	if (infoType == TYPE_RTM)
	{
		Log.trace ("I don't know anything about time... Let me ask the ESP" dendl);
		askForTime();
	}
	else if (infoType == TYPE_RIF)
	{
		variableChange.sendInfo(); // We send the variables values to the ESP8266
	}
} // readSerial

ArduinoSerial serial = ArduinoSerial();
