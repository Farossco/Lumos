#include "Bluetooth.h"

Bluetooth::Bluetooth() : module (&Serial3)
{ }

void Bluetooth::init ()
{
	module.updateStatus();
}

void Bluetooth::action ()
{
	module.action();
}

Bluetooth bluetooth = Bluetooth();
