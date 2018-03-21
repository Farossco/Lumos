#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <BM70.h>

class Bluetooth
{
public:
	Bluetooth();
	void testInit ();
	void testLoop ();

private:
	BM70 module;
};

extern Bluetooth bluetooth;

#endif // ifndef BLUETOOTH_H
