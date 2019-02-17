#include <Arduino.h>

#if defined(ESP8266_PERI_H_INCLUDED)

# include <Time.h>
# include "ESPSerial.h"
# include "Logger.h"
# include "Request.h"
# include "Utils.h"
# include "TimeLib.h"

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
	uint8_t length;

	if (Serial.available())
		length = Serial.readBytesUntil ('z', buf, bufSize);
	else if (Serial1.available())
		length = Serial1.readBytesUntil ('z', buf, bufSize);

	buf[length] = '\0';

	request.decode (buf, SOURCE_ESP8266_SERIAL);
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

#endif // if defined(ESP8266_PERI_H_INCLUDED)
