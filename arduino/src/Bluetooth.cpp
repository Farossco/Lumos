#include "Bluetooth.h"

Bluetooth::Bluetooth()
{
	module = BM70(&Serial3);
}

void Bluetooth::testInit ()
{ }

void Bluetooth::testLoop ()
{

}

Bluetooth bluetooth = Bluetooth();
