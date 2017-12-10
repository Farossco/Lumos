#ifndef ARDUINO_SERIAL_H
#define ARDUINO_SERIAL_H

#include <Arduino.h>

class ArduinoSerial
{
public:
	void begin (long serialBaudRate);
	void begin (long serialBaudRate, long serial1BaudRate);
	void begin (long serialBaudRate, long serial1BaudRate, long serial2BaudRate);
	void begin (long serialBaudRate, long serial1BaudRate, long serial2BaudRate, long serial3BaudRate);
	void waitForTime ();
	void askForTime (); // Asking for time to the ESP8266 (via internet)
	void read ();       // Receive datas from ESP8266 for Wi-Wi control
};

extern ArduinoSerial serial;

#endif // ifndef ARDUINO_SERIAL_H
