#include "arduino.h"

void testVariableChange ()
{
	if (changeOn != on)
	{
		printlnNoPrefix();
		print ("\"On\" changed from ");
		printNoPrefix (changeOn ? "True" : "False");
		printNoPrefix (" to ");
		printlnNoPrefix (on ? "True" : "False");
		changeOn = on;
		sendInfo();
	}

	if (changeRgb != rgb[MODE_DEFAULT])
	{
		printlnNoPrefix();
		print ("\"RGB\" of DEFAULT changed from ");
		printNoPrefix (changeRgb, HEX);
		printNoPrefix (" to ");
		printlnNoPrefix (rgb[MODE_DEFAULT], HEX);
		changeRgb = rgb[MODE_DEFAULT];
		sendInfo();
		eepromWrite();
	}

	for (int i = 0; i <= MODE_MAX; i++)
		if (changePower[i] != power[i])
		{
			printlnNoPrefix();
			print ("\"Power\" of " + modeName (i) + " changed from ");
			printNoPrefix ((int) changePower[i], DEC);
			printNoPrefix (" to ");
			printlnNoPrefix ((int) power[i], DEC);
			changePower[i] = power[i];
			sendInfo();
			if (i != MODE_WAKEUP)
				eepromWrite();
		}

	for (int i = 0; i <= MODE_MAX; i++)
		if (changeSpeed[i] != speed[i])
		{
			printlnNoPrefix();
			print ("\"Speed\" of " + modeName (i) + " changed from ");
			printNoPrefix (changeSpeed[i], DEC);
			printNoPrefix (" to ");
			printlnNoPrefix (speed[i], DEC);
			changeSpeed[i] = speed[i];
			sendInfo();
			eepromWrite();
		}

	if (changeMode != mode)
	{
		printlnNoPrefix();
		print ("\"Mode\" changed from ");
		printNoPrefix (modeName (changeMode));
		printNoPrefix (" to ");
		printlnNoPrefix (modeName (mode));
		changeMode = mode;
		sendInfo();
		eepromWrite();
	}
} // testVariableChange

void initVariableChange ()
{
	// Initializing to default values
	changeOn   = on;
	changeRgb  = rgb[MODE_DEFAULT];
	changeMode = mode;
	for (int i = 0; i <= MODE_MAX; i++)
		changePower[i] = power[i];
	for (int i = 0; i <= MODE_MAX; i++)
		changeSpeed[i] = speed[i];
}

void sendInfo ()
{
	printlnNoPrefix();
	println ("Sending variables infos to the ESP8266");

	Serial1.print ("ONF");
	Serial1.print (on ? 1 : 0, DEC);
	Serial1.print ("z");

	Serial1.print ("MOD");
	Serial1.print (mode, DEC);
	Serial1.print ("z");

	Serial1.print ("RGB0");
	Serial1.print (rgb[0], HEX);
	Serial1.print ("z");

	for (int i = 0; i <= MODE_MAX; i++)
	{
		Serial1.print ("POW");
		Serial1.print (i);
		Serial1.print ((int) power[i], DEC);
		Serial1.print ("z");

		Serial1.print ("SPE");
		Serial1.print (i);
		Serial1.print (speed[i], DEC);
		Serial1.print ("z");
	}
}
