#include <Arduino.h>

#if defined(LUMOS_ESP8266)

# include <Time.h>
# include "ESPSerial.h"
# include "Logger.h"
# include "Request.h"
# include "Utils.h"
# include "TimeLib.h"
# include "Json.h"

void ESPSerial::init (long debugSerialrate, long comSerialRate)
{
	debugSerial.begin (debugSerialrate); // Initialize debug communication
	comSerialRx.begin (comSerialRate);   // Initialize arduino Rx communication
	comSerialTx.begin (comSerialRate);   // Initialize arduino Tx communication
}

// Receive data from Arduino
void ESPSerial::receiveAndDecode ()
{
	if (!comSerialRx.available())
		return;

	String str = comSerialRx.readStringUntil ('z');

	RequestData requestData = request.decode (str);

	if (requestData.type == requestTime)
		serial.sendTime();  // We send the time to the Arduino
	else if (requestData.type == requestInfos)
		Log.verbose (json.get ("OK", "").c_str());
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

		comSerialTx.print (str.c_str());
	}
	else
	{
		Log.warning ("Time not set, not sending time to arduino..." dendl);
	}
}

void ESPSerial::getInfos ()
{
	comSerialTx.print ("INFOz");
}

ESPSerial serial = ESPSerial();

#endif // if defined(LUMOS_ESP8266)