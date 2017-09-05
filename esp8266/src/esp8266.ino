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
	for (int i=0; i<=N_PIN; i++)
	pinMode (i, OUTPUT);

	for (int i=0; i<=N_PIN; i++)
		digitalWrite (2, LOW);
}
