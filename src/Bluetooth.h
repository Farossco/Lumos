#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <BM70.h>

class Bluetooth
{
public:
	Bluetooth();
	void init ();
	void action ();
	void makeConnection ();
	bool isEnabled ();

private:
	BM70 module;
	uint32_t counter;
	uint8_t connectionState;
};

extern Bluetooth bluetooth;

#endif // ifndef BLUETOOTH_H
