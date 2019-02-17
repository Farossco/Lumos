#ifndef ARDUINO_SERIAL_H
#define ARDUINO_SERIAL_H

#include <Arduino.h>

class ArduinoSerial
{
public:
	void init (uint32_t serialBaudRate);
	void init (uint32_t serialBaudRate, uint32_t serial1BaudRate);
	void init (uint32_t serialBaudRate, uint32_t serial1BaudRate, uint32_t serial2BaudRate);
	void init (uint32_t serialBaudRate, uint32_t serial1BaudRate, uint32_t serial2BaudRate, uint32_t serial3BaudRate);
	void waitForTime ();
	static void askForTime (); // Asking for time to the ESP8266 (via internet)
	void receiveAndDecode ();  // Receive datas from ESP8266 for Wi-Fi control
};

extern ArduinoSerial serial;

#endif // ifndef ARDUINO_SERIAL_H
