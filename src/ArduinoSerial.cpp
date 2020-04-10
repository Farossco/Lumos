#if defined(LUMOS_ARDUINO_MEGA)

#include <Time.h>
#include "ArduinoSerial.h"
#include "Logger.h"
#include "Request.h"
#include "VariableChange.h"
#include "Alarms.h"

void ArduinoSerial::init (uint32_t serialBaudRate)
{
	debugSerial.begin (serialBaudRate); // Initialize debug communication
}

void ArduinoSerial::init (uint32_t serialBaudRate, uint32_t serial1BaudRate)
{
	debugSerial.begin (serialBaudRate); // Initialize debug communication
	comSerial.begin (serial1BaudRate);  // Initialize ESP8266 communication

	debugSerial.println ("\n\n");
}

void ArduinoSerial::waitForTime ()
{
	time_t lastMillis = millis() - 5000;

	inf << "Waiting to receive time..." << dendl;

	while (timeStatus() == timeNotSet) // Doesn't start if time isn't set
	{
		receiveAndDecode();

		if (millis() - lastMillis >= 5000)
		{
			askForTime();
			lastMillis = millis();
		}
	}
	trace << "Time Received" << dendl;
}

// Asking for time to the ESP8266 (via internet)
void ArduinoSerial::askForTime ()
{
	trace << "Kindly asking ESP for time" << dendl;
	comSerial.print ("TIMEPLEASEz"); // z is the end character
}

// Receive data from ESP8266 for Wi-Wi control
void ArduinoSerial::receiveAndDecode ()
{
	Stream * serialInput;

	if (!(serialInput = &debugSerial)->available() && !(serialInput = &comSerial)->available())
		return;

	String str = serialInput->readStringUntil ('z');

	RequestData requestData = request.decode (str);

	if (requestData.type == requestTime)
	{
		trace << "I don't know anything about time... Let me ask the ESP" << dendl;
		serial.askForTime();
	}
	else if (requestData.type == requestInfos)
	{
		variableChange.sendInfo(); // We send the variables values to the ESP8266
	}
}

ArduinoSerial serial = ArduinoSerial();

#endif // if defined(LUMOS_ARDUINO_MEGA)