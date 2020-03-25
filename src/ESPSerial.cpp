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
	Serial.begin (serialBaudRate);   // Initialize debug communication
	Serial1.begin (serial1BaudRate); // Initialize arduino communication
}

// Receive data from Arduino
void ESPSerial::receiveAndDecode ()
{
	if (!Serial.available())
		return;

	String str = Serial.readStringUntil ('z');

	RequestData requestData = request.decode (str);

	if (requestData.type == requestTime)
		serial.sendTime();  // We send the time to the Arduino
	else if (requestData.type == requestInfos)
		Log.verbose(json.get("OK", "").c_str());
} // ESPSerial::receiveAndDecode

void ESPSerial::sendTime ()
{
	Log.trace ("Time is: %s" endl, utils.clock().c_str());

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