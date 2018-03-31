#ifndef ARDUINO_SERIAL_H
#define ARDUINO_SERIAL_H

#include <Arduino.h>

const boolean WAIT_FOR_TIME = true; // If we have to wait for time sync (if true, program will not start until time is synced)

class ArduinoSerial
{
public:
	void init (long serialBaudRate);
	void init (long serialBaudRate, long serial1BaudRate);
	void init (long serialBaudRate, long serial1BaudRate, long serial2BaudRate);
	void init (long serialBaudRate, long serial1BaudRate, long serial2BaudRate, long serial3BaudRate);
	void waitForTime ();
	static void askForTime (); // Asking for time to the ESP8266 (via internet)
	void read ();              // Receive datas from ESP8266 for Wi-Wi control
};

extern ArduinoSerial serial;

#endif // ifndef ARDUINO_SERIAL_H
