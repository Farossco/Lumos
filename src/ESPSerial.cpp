#include <Arduino.h>

#if defined(LUMOS_ESP8266)

# include <Time.h>
# include "ESPSerial.h"
# include "Logger.h"
# include "Request.h"
# include "Utils.h"
# include "TimeLib.h"
# include "Json.h"

void ESPSerial::init (long serialBaudRate, long serial1BaudRate)
{
	Serial.begin (serialBaudRate);  // Initialize debug communication
	Serial1.begin (serial1BaudRate); // Initialize arduino communication
}

// Receive data from Arduino
void ESPSerial::receiveAndDecode ()
{
	if (!Serial.available())
		return;

	String str = Serial.readStringUntil ('z');

	Req requestData = request.decode (str);

	if (requestData.type == requestTime)
		serial.sendTime();  // We send the time to the Arduino
	else if (requestData.type == requestInfos)
		if (Log.isEnabledFor (LEVEL_INFO))
			json.send ("OK", "", &Serial, false);
} // ESPSerial::receiveAndDecode

void ESPSerial::sendTime ()
{
	char buf[30];

	Log.trace ("Time is: %s" dendl, utils.clock (buf));

	if (timeStatus() != timeNotSet)
	{
		String str = "TIM";
		str += now();
		str += 'z';

		Log.trace ("Sending time to arduino: %s" dendl, str.c_str());

		Serial1.print (str.c_str());
	}
	else
	{
		Log.warning ("Time not set, not sending time to arduino..." dendl);
	}
}

ESPSerial serial = ESPSerial();

#endif // if defined(LUMOS_ESP8266)
