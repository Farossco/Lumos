#ifndef ARDUINO_SERIAL_H
#define ARDUINO_SERIAL_H

#include <Arduino.h>

class SerialCom
{
public:
	void init (uint32_t debugBaudRate, uint32_t comBaudRate);
	void receiveAndDecode ();
	void waitForTime ();
	void sendTime ();
	void getTime ();
	void sendData ();
	void getData ();

	#if defined(LUMOS_ARDUINO_MEGA)
	HardwareSerial &comSerialRx = Serial1;
	HardwareSerial &comSerialTx = Serial1;
	#endif // if defined(LUMOS_ARDUINO_MEGA)
	#if defined(LUMOS_ESP8266)
	HardwareSerial &comSerialRx = Serial;
	HardwareSerial &comSerialTx = Serial1;
	#endif // if defined(LUMOS_ARDUINO_MEGA)

	HardwareSerial &debugSerial = Serial;
};

extern SerialCom serial;


#endif // ifndef ARDUINO_SERIAL_H