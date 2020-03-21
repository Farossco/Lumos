#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#if defined(LUMOS_ARDUINO_MEGA)

# include <BM70.h>

const uint8_t PIN_BLE_LED_RED     = 47; // RGB Red pin
const uint8_t PIN_BLE_LED_GREEN   = 44; // RGB Green pin
const uint8_t PIN_BLE_LED_BLUE    = 43; // RGB Blue pin
const uint8_t BLE_INDICATOR_POWER = 10;

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

#endif // if defined(LUMOS_ARDUINO_MEGA)

#endif // ifndef BLUETOOTH_H