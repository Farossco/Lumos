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

	Request request = comSerialRx.readStringUntil ('z');

	request.process();

	if (request.getType() == requestTime)
		serial.sendTime();  // We send the time to the Arduino
	else if (request.getType() == requestInfos)
		verb << json.getData ("OK", "");
} // ESPSerial::receiveAndDecode

void ESPSerial::sendTime ()
{
	trace << "Time is: " << utils.getClock() << endl;

	if (timeStatus() != timeNotSet)
	{
		String str = "TIM";
		str += now();
		str += 'z';

		trace << "Sending time to arduino: " << str << endl;

		comSerialTx.print (str.c_str());

		trace << np << endl;
	}
	else
	{
		warn << "Time not set, not sending time to arduino..." << dendl;
	}
}

void ESPSerial::getInfos ()
{
	comSerialTx.print ("INFOz");
}

ESPSerial serial = ESPSerial();

#endif // if defined(LUMOS_ESP8266)