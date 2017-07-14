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


	for (int i = MODE_MIN; i < N_MODE; i++)
		if (changePower[i] != power[i])
		{
			printlnNoPrefix();
			print ("\"Power\" of " + modeName (i, CAPS_NONE) + " changed from ");
			printNoPrefix ((int) changePower[i], DEC);
			printNoPrefix (" to ");
			printlnNoPrefix ((int) power[i], DEC);
			changePower[i] = power[i];
			sendInfo();
			if (i != MODE_WAKEUP)
				eepromWrite();
		}


	for (int i = MODE_MIN; i < N_MODE; i++)
		if (changeSpeed[i] != speed[i])
		{
			printlnNoPrefix();
			print ("\"Speed\" of " + modeName (i, CAPS_NONE) + " changed from ");
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
		printNoPrefix (modeName (changeMode, CAPS_NONE));
		printNoPrefix (" to ");
		printlnNoPrefix (modeName (mode, CAPS_NONE));
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
	for (int i = MODE_MIN; i < N_MODE; i++)
		changePower[i] = power[i];
	for (int i = MODE_MIN; i < N_MODE; i++)
		changeSpeed[i] = speed[i];
}

void sendInfo ()
{
	printlnNoPrefix();
	print ("Sending variables infos to the ESP8266 (");

	Serial1.print ("ONF");
	Serial1.print (on ? 1 : 0, DEC);
	Serial1.print ("z");
	printNoPrefix ("ONF");
	printNoPrefix (on ? 1 : 0, DEC);
	printNoPrefix ("z ");

	Serial1.print ("MOD");
	Serial1.print (mode, DEC);
	Serial1.print ("z");
	printNoPrefix ("MOD");
	printNoPrefix (mode, DEC);
	printNoPrefix ("z ");

	Serial1.print ("RGB");
	Serial1.print (rgb[MODE_DEFAULT], HEX);
	Serial1.print ("z");
	printNoPrefix ("RGB");
	printNoPrefix (rgb[MODE_DEFAULT], HEX);
	printNoPrefix ("z ");

	for (int i = MODE_MIN; i < N_MODE; i++)
	{
		Serial1.print ("POW");
		Serial1.print (i, DEC);
		Serial1.print ((int) power[i], DEC);
		Serial1.print ("z");
		printNoPrefix ("POW");
		printNoPrefix (i, DEC);
		printNoPrefix ((int) power[i], DEC);
		printNoPrefix ("z ");
	}

	for (int i = MODE_MIN; i < N_MODE; i++)
	{
		Serial1.print ("SPE");
		Serial1.print (i, DEC);
		Serial1.print ((((long) speed[i] - (MIN_SPEED[i] - SEEKBAR_MIN)) * (SEEKBAR_MAX - SEEKBAR_MIN)) / (MAX_SPEED[i] - MIN_SPEED[i]), DEC);
		Serial1.print ("z");
		printNoPrefix ("SPE");
		printNoPrefix (i, DEC);
		printNoPrefix ((((long) speed[i] - (MIN_SPEED[i] - SEEKBAR_MIN)) * (SEEKBAR_MAX - SEEKBAR_MIN)) / (MAX_SPEED[i] - MIN_SPEED[i]), DEC);
		printNoPrefix ("z ");
	}

	printlnNoPrefix (")");
} // sendInfo
