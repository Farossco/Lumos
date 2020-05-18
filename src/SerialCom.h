#ifndef ARDUINO_SERIAL_H
#define ARDUINO_SERIAL_H

#include <Arduino.h>

class SerialCom
{
public:
	void init (uint32_t debugBaudRate, uint32_t comBaudRate);
	void receiveAndDecode ();
	bool checkTime ();
	void sendTime ();
	void getTime ();
	void sendData ();
	void getData ();

	HardwareSerial &comSerial = Serial1;
	HardwareSerial &debugSerial = Serial;
};

extern SerialCom serial;


#endif // ifndef ARDUINO_SERIAL_H