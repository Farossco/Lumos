#include "esp8266.h"

void setup ()
{
	initGpios();

	initSerial();

	initWifiServer();

	sendTime();
}

void loop ()
{
	readSerial();

	readWeb();
}

void initGpios ()
{
	pinMode (0, OUTPUT);
	pinMode (2, OUTPUT);
	digitalWrite (0, LOW);
	digitalWrite (2, LOW);
}
