#if defined(LUMOS_ARDUINO_MEGA)

#include <Time.h>
#include "ArduinoSerial.h"
#include "Logger.h"
#include "Request.h"
#include "VariableChange.h"
#include "Alarms.h"

void ArduinoSerial::init (uint32_t serialBaudRate)
{
	Serial.begin (serialBaudRate); // Initialize debug communication
}

void ArduinoSerial::init (uint32_t serialBaudRate, uint32_t serial1BaudRate)
{
	Serial.begin (serialBaudRate);   // Initialize debug communication
	Serial1.begin (serial1BaudRate); // Initialize ESP8266 communication
}

void ArduinoSerial::init (uint32_t serialBaudRate, uint32_t serial1BaudRate, uint32_t serial2BaudRate)
{
	Serial.begin (serialBaudRate);   // Initialize debug communication
	Serial1.begin (serial1BaudRate); // Initialize ESP8266 communication
	Serial2.begin (serial2BaudRate); // Initialize DFPlayer communication
}

void ArduinoSerial::init (uint32_t serialBaudRate, uint32_t serial1BaudRate, uint32_t serial2BaudRate, uint32_t serial3BaudRate)
{
	Serial.begin (serialBaudRate);   // Initialize debug communication
	Serial1.begin (serial1BaudRate); // Initialize ESP8266 communication
	Serial2.begin (serial2BaudRate); // Initialize DFPlayer communication
	Serial3.begin (serial3BaudRate); // Initialize DFPlayer communication
}

void ArduinoSerial::waitForTime ()
{
	askForTime();

	time_t lastMillis = millis();

	while (timeStatus() == timeNotSet) // Doesn't start if time isn't set
	{
		receiveAndDecode();

		if (millis() - lastMillis >= 5000)
		{
			if (timeStatus() == timeNotSet)
				Log.verbose ("Time is not set" endl);

			askForTime();
			lastMillis = millis();
		}
	}
	Log.trace ("Time Received" dendl);
}

// Asking for time to the ESP8266 (via internet)
void ArduinoSerial::askForTime ()
{
	Log.verbose ("Kindly asking ESP for time" dendl);
	Serial1.print ("TIMEPLEASEz"); // z is the end character
}

// Receive datas from ESP8266 for Wi-Wi control
void ArduinoSerial::receiveAndDecode ()
{
	if (!Serial.available() && !Serial1.available())
		return;

	size_t bufSize = 14;
	char buf[bufSize];

	uint8_t length;

	if (Serial.available())
		length = Serial.readBytesUntil ('z', buf, bufSize);
	else if (Serial1.available())
		length = Serial1.readBytesUntil ('z', buf, bufSize);

	buf[length] = '\0';

	request.decode (buf, SOURCE_ARDUINO_SERIAL);
}

ArduinoSerial serial = ArduinoSerial();

#endif // if defined(LUMOS_ARDUINO_MEGA)
