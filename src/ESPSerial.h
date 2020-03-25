#ifndef ESP_SERIAL_H
#define ESP_SERIAL_H

#if defined(LUMOS_ESP8266)

# include <Arduino.h>

# define RX_TIMEOUT       200
# define RX_TIMEOUT_DEBUG 5000

class ESPSerial
{
public:
	void init (long comSerialRate, long debugSerialrate);
	void receiveAndDecode ();
	void sendTime ();
	void getInfos ();

	HardwareSerial &comSerialRx = Serial;
	HardwareSerial &comSerialTx = Serial1;
	HardwareSerial &debugSerial = Serial;
};

extern ESPSerial serial;

#endif // if defined(LUMOS_ESP8266)

#endif // ifndef ESP_SERIAL_H