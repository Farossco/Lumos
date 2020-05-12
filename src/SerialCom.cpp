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

bool SerialCom::checkTime ()
{
	getTime();
	delay (10);
	receiveAndDecode();

	return (timeStatus() != timeNotSet);
}

void SerialCom::receiveAndDecode ()
{
	Stream * serialInput;

	if ((serialInput = &debugSerial)->available() || (serialInput = &comSerialRx)->available())
	{
		serialInput->setTimeout (serialInput == &debugSerial ? 2000 : 50);

		Request request (serialInput->readStringUntil ('z'));

		request.process();

		if (request.getType() == RequestType::requestTime)
			serial.sendTime();
		else if (request.getType() == RequestType::requestData)
			serial.sendData();
	}
}

void SerialCom::sendTime ()
{
	#if defined(LUMOS_ARDUINO_MEGA)
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
	trace << "Time Request sent" << dendl;
	comSerialTx.print (RequestType (RequestType::requestTime).toString (true) + 'z'); // z is the end character
}

void SerialCom::sendData ()
{
	#if defined(LUMOS_ARDUINO_MEGA)
	variableChange.sendData();
	#endif

	#if defined(LUMOS_ESP8266)
	inf << json.getData() << dendl;
	#endif
}

void SerialCom::getData ()
{
	comSerialTx.print (RequestType (RequestType::requestData).toString (true) + 'z');
}

SerialCom serial = SerialCom();