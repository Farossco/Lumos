#include <TimeLib.h>
#include "SerialCom.h"
#include "ArduinoLogger.h"
#include "Request.h"
#include "VariableChange.h"
#include "Alarms.h"
#include "Utils.h"
#include "Json.h"
#include "wifi_com.h"

void SerialCom::init(uint32_t debugBaudRate, uint32_t comBaudRate)
{
	debugSerial.begin (debugBaudRate); /* Initialize debug communication */
	comSerial.begin (comBaudRate);     /* Initialize arduino Rx communication */

	#if defined(LUMOS_ESP32)
	comSerial.setRxBufferSize (1024);
	#endif /* if defined(LUMOS_ESP32) */

	debugSerial.println ("\n\n");

	logger.add (debugSerial, DEBUG_LEVEL);
}

bool SerialCom::checkTime()
{
	getTime();
	delay (10);
	receiveAndDecode();

	return (timeStatus() != timeNotSet);
}

void SerialCom::receiveAndDecode()
{
	Stream * serialInput;

	if ((serialInput = &debugSerial)->available() || (serialInput = &comSerial)->available()) {
		serialInput->setTimeout (serialInput == &debugSerial ? 2000 : 50);

		Request request (serialInput->readStringUntil ('z'));

		request.process();

		if (request.getType() == RequestType::requestTime)
			serial.sendTime();
		else if (request.getType() == RequestType::requestData)
			serial.sendData();
	}
}

void SerialCom::sendTime()
{
	#if defined(LUMOS_ARDUINO_MEGA)
	serial.getTime();
	#endif /* if defined(LUMOS_ARDUINO_MEGA) */

	#if defined(LUMOS_ESP32)
	trace << "Time is: " << utils.getClock() << endl;

	if (timeStatus() != timeNotSet) {
		String str = String ("TIM") + now() + 'z';

		trace << "Sending time to arduino: " << str << endl;

		comSerial.print (str.c_str());

		trace << np << endl;
	} else {
		warn << "Time not set, new attempt" << dendl;

		wifi_com_time_get();
	}
	#endif // if defined(LUMOS_ESP32)
}

/* Getting time from the ESP8266 (via internet) */
void SerialCom::getTime()
{
	trace << "Time Request sent" << dendl;
	comSerial.print (RequestType (RequestType::requestTime).toString (true) + 'z'); /* z is the end character */
}

void SerialCom::sendData()
{
	#if defined(LUMOS_ARDUINO_MEGA)
	variableChange.sendData();
	#endif /* if defined(LUMOS_ARDUINO_MEGA) */

	#if defined(LUMOS_ESP32)
	inf << json.getData() << dendl;
	#endif /* if defined(LUMOS_ESP32) */
}

void SerialCom::getData()
{
	comSerial.print (RequestType (RequestType::requestData).toString (true) + 'z');
}

SerialCom serial = SerialCom();
