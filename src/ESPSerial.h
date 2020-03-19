#ifndef ESP_SERIAL_H
#define ESP_SERIAL_H

#if defined(LUMOS_ESP8266)

# include <Arduino.h>

# define RX_TIMEOUT       200
# define RX_TIMEOUT_DEBUG 5000

class ESPSerial
{
public:
	void init (long serialBaudRate, long serial1BaudRate);
	void receiveAndDecode ();
	void sendTime ();
};

extern ESPSerial serial;

#endif // if defined(LUMOS_ESP8266)

#endif // ifndef ESP_SERIAL_H
