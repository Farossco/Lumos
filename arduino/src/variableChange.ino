#include "arduino.h"

void testVariableChange ()
{
	boolean flagSendInfo    = false;
	boolean flagWriteEeprom = false;

	if (changeOn != on)
	{
		printlnNoPrefix(LEVEL_DEBUG);
		print (LEVEL_DEBUG, "\"On\" changed from ");
		printNoPrefix (LEVEL_DEBUG, changeOn ? "True" : "False");
		printNoPrefix (LEVEL_DEBUG, " to ");
		printlnNoPrefix (LEVEL_DEBUG, on ? "True" : "False");
		changeOn     = on;
		flagSendInfo = true;
	}

	if (changeRgb != rgb[MODE_DEFAULT])
	{
		printlnNoPrefix(LEVEL_DEBUG);
		print (LEVEL_DEBUG, "\"RGB\" of DEFAULT changed from ");
		printNoPrefix (LEVEL_DEBUG, changeRgb, HEX);
		printNoPrefix (LEVEL_DEBUG, " to ");
		printlnNoPrefix (LEVEL_DEBUG, rgb[MODE_DEFAULT], HEX);
		changeRgb       = rgb[MODE_DEFAULT];
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}


	for (int i = MODE_MIN; i < N_MODE; i++)
		if (changePower[i] != power[i])
		{
			printlnNoPrefix(LEVEL_DEBUG);
			print (LEVEL_DEBUG, "\"Power\" of " + modeName (i, CAPS_NONE) + " changed from ");
			printNoPrefix (LEVEL_DEBUG, (int) changePower[i], DEC);
			printNoPrefix (LEVEL_DEBUG, " to ");
			printlnNoPrefix (LEVEL_DEBUG, (int) power[i], DEC);
			changePower[i] = power[i];
			flagSendInfo   = true;
			if (i != MODE_WAKEUP)
				flagWriteEeprom = true;
		}


	for (int i = MODE_MIN; i < N_MODE; i++)
		if (changeSpeed[i] != speed[i])
		{
			printlnNoPrefix(LEVEL_DEBUG);
			print (LEVEL_DEBUG, "\"Speed\" of " + modeName (i, CAPS_NONE) + " changed from ");
			printNoPrefix (LEVEL_DEBUG, changeSpeed[i], DEC);
			printNoPrefix (LEVEL_DEBUG, " to ");
			printlnNoPrefix (LEVEL_DEBUG, speed[i], DEC);
			changeSpeed[i]  = speed[i];
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}

	if (changeMode != mode)
	{
		printlnNoPrefix(LEVEL_DEBUG);
		print (LEVEL_DEBUG, "\"Mode\" changed from ");
		printNoPrefix (LEVEL_DEBUG, modeName (changeMode, CAPS_NONE));
		printNoPrefix (LEVEL_DEBUG, " to ");
		printlnNoPrefix (LEVEL_DEBUG, modeName (mode, CAPS_NONE));
		changeMode      = mode;
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	if (flagSendInfo)
		sendInfo();

	if (flagWriteEeprom)
		eepromWrite();
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
	printlnNoPrefix(LEVEL_DEBUG);
	println (LEVEL_DEBUG, "Sending variables infos to the ESP8266");
	print(LEVEL_DEBUG, "(");

	Serial1.print ("ONF");
	Serial1.print (on ? 1 : 0, DEC);
	Serial1.print ("z");
	printNoPrefix (LEVEL_DEBUG, "ONF");
	printNoPrefix (LEVEL_DEBUG, on ? 1 : 0, DEC);
	printNoPrefix (LEVEL_DEBUG, "z ");

	Serial1.print ("MOD");
	Serial1.print (mode, DEC);
	Serial1.print ("z");
	printNoPrefix (LEVEL_DEBUG, "MOD");
	printNoPrefix (LEVEL_DEBUG, mode, DEC);
	printNoPrefix (LEVEL_DEBUG, "z ");

	Serial1.print ("RGB");
	Serial1.print (rgb[MODE_DEFAULT], HEX);
	Serial1.print ("z");
	printNoPrefix (LEVEL_DEBUG, "RGB");
	printNoPrefix (LEVEL_DEBUG, rgb[MODE_DEFAULT], HEX);
	printNoPrefix (LEVEL_DEBUG, "z ");

	for (int i = MODE_MIN; i < N_MODE; i++)
	{
		Serial1.print ("POW");
		Serial1.print (i, DEC);
		Serial1.print ((int) power[i], DEC);
		Serial1.print ("z");
		printNoPrefix (LEVEL_DEBUG, "POW");
		printNoPrefix (LEVEL_DEBUG, i, DEC);
		printNoPrefix (LEVEL_DEBUG, (int) power[i], DEC);
		printNoPrefix (LEVEL_DEBUG, "z ");
	}

	for (int i = MODE_MIN; i < N_MODE; i++)
	{
		Serial1.print ("SPE");
		Serial1.print (i, DEC);
		Serial1.print ((((long) speed[i] - (MIN_SPEED[i] - SEEKBAR_MIN)) * (SEEKBAR_MAX - SEEKBAR_MIN)) / (MAX_SPEED[i] - MIN_SPEED[i]), DEC);
		Serial1.print ("z");
		printNoPrefix (LEVEL_DEBUG, "SPE");
		printNoPrefix (LEVEL_DEBUG, i, DEC);
		printNoPrefix (LEVEL_DEBUG, (((long) speed[i] - (MIN_SPEED[i] - SEEKBAR_MIN)) * (SEEKBAR_MAX - SEEKBAR_MIN)) / (MAX_SPEED[i] - MIN_SPEED[i]), DEC);
		printNoPrefix (LEVEL_DEBUG, "z ");
	}

	printNoPrefix (LEVEL_DEBUG, ")");
} // sendInfo
