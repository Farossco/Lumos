#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <BM70.h>

const int PIN_BLE_LED_RED     = 4; // RGB Red pin
const int PIN_BLE_LED_GREEN   = 5; // RGB Green pin
const int PIN_BLE_LED_BLUE    = 6; // RGB Blue pin
const int BLE_INDICATOR_POWER = 10;

class Bluetooth
{
public:
	Bluetooth();
	void init ();
	void action ();
	void makeConnection ();
	bool isEnabled ();

	void lightConnected ();
	void lightIdle ();
	void lightOff ();
private:
	BM70 module;
	uint32_t counter;
	uint8_t connectionState;
};

extern Bluetooth bluetooth;

#endif // ifndef BLUETOOTH_H
