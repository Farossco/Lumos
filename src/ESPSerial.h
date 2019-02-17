#ifndef ESP_SERIAL_H
#define ESP_SERIAL_H

#include <Arduino.h>

#if defined(ESP8266_PERI_H_INCLUDED)

class ESPSerial
{
public:
	void init (long serialBaudRate);
	void receiveAndDecode ();
	void sendTime ();
};

extern ESPSerial serial;

#endif // if defined(ESP8266_PERI_H_INCLUDED)

#endif // ifndef ESP_SERIAL_H
