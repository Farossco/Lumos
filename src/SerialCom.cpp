#include <Time.h>
#include "SerialCom.h"
#include "ArduinoLogger.h"
#include "Request.h"
#include "VariableChange.h"
#include "Alarms.h"
#include "Utils.h"
#include "Json.h"
#include "Wifi.h"

void SerialCom::init (uint32_t debugBaudRate, uint32_t comBaudRate)
{
	debugSerial.begin (debugBaudRate); // Initialize debug communication

	comSerialRx.begin (comBaudRate); // Initialize arduino Rx communication
	comSerialTx.begin (comBaudRate); // Initialize arduino Tx communication

	#if defined(LUMOS_ESP8266)
	comSerialRx.setRxBufferSize (1024);
	#endif

	debugSerial.println ("\n\n");

	logger.add (debugSerial, DEBUG_LEVEL);
}

void SerialCom::waitForTime ()
{
	time_t lastMillis = millis() - 5000;

	inf << "Waiting to receive time..." << dendl;

	while (timeStatus() == timeNotSet) // Doesn't start if time isn't set
	{
		receiveAndDecode();

		if (millis() - lastMillis >= 5000)
		{
			getTime();
			lastMillis = millis();
		}
	}

	trace << "Time Received" << dendl;
}

void SerialCom::receiveAndDecode ()
{
	Stream * serialInput;

	if ((serialInput = &debugSerial)->available() || (serialInput = &comSerialRx)->available())
	{
		Request request (serialInput->readStringUntil ('z'));

		request.process();

		if (request.getType() == RequestType::requestTime)
			serial.sendTime();
		else if (request.getType() == RequestType::requestInfos)
			serial.sendData();
	}
}

void SerialCom::sendTime ()
{
	#if defined(LUMOS_ARDUINO_MEGA)
	trace << "I don't know anything about time... Let me ask the ESP" << dendl;
	serial.getTime();
	#endif

	#if defined(LUMOS_ESP8266)
	trace << "Time is: " << utils.getClock() << endl;

	if (timeStatus() != timeNotSet)
	{
		String str = String ("TIM") + now() + 'z';

		trace << "Sending time to arduino: " << str << endl;

		comSerialTx.print (str.c_str());

		trace << np << endl;
	}
	else
	{
		warn << "Time not set, new attempt" << dendl;

		wifi.getTime();
	}
	#endif // if defined(LUMOS_ESP8266)
}

// Getting time from the ESP8266 (via internet)
void SerialCom::getTime ()
{
	trace << "Kindly asking ESP for time" << dendl;
	comSerialTx.print ("TIMEPLEASEz"); // z is the end character
}

void SerialCom::sendData ()
{
	#if defined(LUMOS_ARDUINO_MEGA)
	variableChange.sendData();
	#endif

	#if defined(LUMOS_ESP8266)
	verb << json.getData ("OK", "");
	#endif
}

void SerialCom::getData ()
{
	comSerialTx.print ("INFOz");
}

SerialCom serial = SerialCom();