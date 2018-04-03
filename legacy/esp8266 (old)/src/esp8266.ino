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
	pinMode (PIN_D0, OUTPUT);
	pinMode (PIN_D1, OUTPUT);
	pinMode (PIN_D2, OUTPUT);
	pinMode (PIN_D3, OUTPUT);
	pinMode (PIN_D4, OUTPUT);
	pinMode (PIN_D5, OUTPUT);
	pinMode (PIN_D6, OUTPUT);
}
