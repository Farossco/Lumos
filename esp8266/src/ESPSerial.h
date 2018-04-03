#ifndef ESP_SERIAL_H
#define ESP_SERIAL_H

#include <Arduino.h>

class ESPSerial
{
public:
	void init (long serialBaudRate);
	void receiveAndDecode ();
	void sendTime ();
};

extern ESPSerial serial;

#endif // ifndef ESP_SERIAL_H
