#if defined(LUMOS_ARDUINO_MEGA)

#include "Bluetooth.h"
#include "Logger.h"
#include "Request.h"
#include <Arduino.h>
#include "WString.h"

Bluetooth::Bluetooth() : module (&Serial2)
{
	pinMode (PIN_BLE_LED_RED, OUTPUT);
	pinMode (PIN_BLE_LED_GREEN, OUTPUT);
	pinMode (PIN_BLE_LED_BLUE, OUTPUT);

	connectionState = 0;
	lightIdle();
}

void Bluetooth::init ()
{
	Log.info ("Initializing Bluetooth module... ");

	module.reset();

	while (module.getStatus() != BM70_STATUS_IDLE)
	{
		module.reset();
		delay (500);
	}

	module.removePaired (0xFF);

	Log.infonp ("Done" dendl);

	connectionState = 0;
}

void Bluetooth::action ()
{
	module.action();

	makeConnection();

	if (module.transparentAvailable())
	{
		char data[14];

		module.transparentRead (data);

		Log.verbose ("Received data from bluetooth: %s" dendl, data);

		request.decode (data, SOURCE_ARDUINO_BLUETOOTH);
	}
}

void Bluetooth::makeConnection ()
{
	if (module.getStatus() == BM70_STATUS_IDLE)
	{
		connectionState = 0;
		lightIdle();
	}

	switch (connectionState)
	{
		case 0:
			connectionState = 1;
			Log.trace ("Configure auto-connect as master... ");
			module.configureAutoConnect (true, 0x3481F42F22A7, (char *) "123456");
			Log.tracenp ("Done." dendl);

			Log.trace ("Waiting for connection..." dendl);
			counter = millis();
			break;

		case 1:
			if (module.getStatus() == BM70_STATUS_CONNECTED)
			{
				connectionState = 2;
				Log.trace ("Connected!" dendl);
				Log.trace ("Waiting for pairing..." dendl);
				counter = millis();
			}
			else if (millis() - counter >= 60000)
			{
				connectionState = 0;
				Log.trace ("No connection after 1 min, resetting" dendl);
				module.reset();
			}
			break;

		case 2:
			if (module.isPaired())
			{
				connectionState = 3;
				Log.trace ("Pairing succeeded." dendl);
			}
			else if (millis() - counter >= 10000)
			{
				connectionState = 0;
				Log.warning ("Pairing timed out, restarting" dendl);
				module.reset();
			}
			break;

		case 3:
			connectionState = 4;
			Log.trace ("Enabling remote transparent... ");
			module.enableTransparent();
			Log.tracenp ("Done." dendl);

			Log.trace ("Waiting for local transparent to be enabled..." dendl);
			counter = millis();
			break;

		case 4:
			if (module.getStatus() == BM70_STATUS_TRANSCOM)
			{
				connectionState = 5;
				lightConnected();
				Log.trace ("Transparent is enabled, connection completed!" dendl);
			}
			else if (millis() - counter >= 5000)
			{
				connectionState = 0;
				Log.warning ("Enabling local transparent timed out, restarting" dendl);
				module.reset();
			}
			break;

		case 5:
			break;
	}
} // Bluetooth::makeConnection

bool Bluetooth::isEnabled ()
{
	return connectionState == 5;
}

void Bluetooth::lightConnected ()
{
	analogWrite (PIN_BLE_LED_RED, LOW);
	analogWrite (PIN_BLE_LED_GREEN, BLE_INDICATOR_POWER);
	analogWrite (PIN_BLE_LED_BLUE, LOW);
}

void Bluetooth::lightIdle ()
{
	analogWrite (PIN_BLE_LED_RED, LOW);
	analogWrite (PIN_BLE_LED_GREEN, LOW);
	analogWrite (PIN_BLE_LED_BLUE, BLE_INDICATOR_POWER);
}

void Bluetooth::lightOff ()
{
	digitalWrite (PIN_BLE_LED_RED, LOW);
	digitalWrite (PIN_BLE_LED_GREEN, LOW);
	digitalWrite (PIN_BLE_LED_BLUE, LOW);
}

Bluetooth bluetooth = Bluetooth();

#endif // if defined(LUMOS_ARDUINO_MEGA)
