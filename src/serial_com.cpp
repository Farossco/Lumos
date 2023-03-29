#include <TimeLib.h>
#include "serial_com.h"
#include "ArduinoLogger.h"
#include "Request.h"
#include "Alarms.h"
#include "Utils.h"
#include "Json.h"
#include "wifi_com.h"

static HardwareSerial &debugSerial = Serial;

void serial_com_init(uint32_t debugBaudRate, uint32_t comBaudRate)
{
	debugSerial.begin(debugBaudRate); /* Initialize debug communication */

	debugSerial.println("\n\n");

	logger.add(debugSerial, DEBUG_LEVEL);
}

/* TODO: Make this asynchronous */
void serial_com_receive_and_decode(void)
{
	Stream *serialInput;

	if ((serialInput = &debugSerial)->available()) {
		serialInput->setTimeout(serialInput == &debugSerial ? 2000 : 50);

		Request request(serialInput->readStringUntil('z'));

		request.process();
	}
}
