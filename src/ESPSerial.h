#ifndef ESP_SERIAL_H
#define ESP_SERIAL_H

#include <Arduino.h>

#if defined(LUMOS_ESP8266)

class ESPSerial
{
public:
	void init (long serialBaudRate);
	void receiveAndDecode ();
	void sendTime ();
};

extern ESPSerial serial;

#endif // if defined(LUMOS_ESP8266)

#endif // ifndef ESP_SERIAL_H
