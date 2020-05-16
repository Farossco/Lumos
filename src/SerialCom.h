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

	#if defined(LUMOS_ARDUINO_MEGA)
	HardwareSerial &comSerial = Serial1;
	#endif
	#if defined(LUMOS_ESP32)
	HardwareSerial &comSerial = Serial2;
	#endif

	HardwareSerial &debugSerial = Serial;
};

extern SerialCom serial;


#endif // ifndef ARDUINO_SERIAL_H