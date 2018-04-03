#include "ESPSerial.h"
#include "Wifi.h"
#include "Logger.h"

void setup ()
{
	Log.init(&Serial, 115200);

	serial.init(9600);

	wifi.init();

	wifi.getTime();

	serial.sendTime();
}

void loop ()
{
	serial.receiveAndDecode();

	wifi.receiveAndDecode();
}