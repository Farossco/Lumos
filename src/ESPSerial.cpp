#include <Arduino.h>

#if defined(LUMOS_ESP8266)

# include <Time.h>
# include "ESPSerial.h"
# include "ArduinoLogger.h"
# include "Request.h"
# include "Utils.h"
# include "TimeLib.h"
# include "Json.h"
# include "Wifi.h"

void ESPSerial::init (long debugSerialrate, long comSerialRate)
{
	debugSerial.begin (debugSerialrate); // Initialize debug communication
	comSerialRx.begin (comSerialRate);   // Initialize arduino Rx communication
	comSerialTx.begin (comSerialRate);   // Initialize arduino Tx communication

	comSerialRx.setRxBufferSize (1024);

	logger.add (debugSerial, DEBUG_LEVEL);
}

// Receive data from Arduino
void ESPSerial::receiveAndDecode ()
{
	if (!comSerialRx.available())
		return;

	Request request = comSerialRx.readStringUntil ('z');

	request.process();

	if (request.getType() == RequestType::requestTime)
		serial.sendTime();  // We send the time to the Arduino
	else if (request.getType() == RequestType::requestInfos)
		verb << json.getData ("OK", "");
}

void ESPSerial::sendTime ()
{
	trace << "Time is: " << utils.getClock() << endl;

	if (timeStatus() != timeNotSet)
	{
		String str = String("TIM") + now() + 'z';

		trace << "Sending time to arduino: " << str << endl;

		comSerialTx.print (str.c_str());

		trace << np << endl;
	}
	else
	{
		warn << "Time not set, new attempt" << dendl;

		wifi.getTime();
	}
}

void ESPSerial::getInfos ()
{
	comSerialTx.print ("INFOz");
}

ESPSerial serial = ESPSerial();

#endif // if defined(LUMOS_ESP8266)