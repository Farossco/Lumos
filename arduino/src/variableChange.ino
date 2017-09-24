#include "arduino.h"

void testVariableChange ()
{
	boolean flagSendInfo    = false;
	boolean flagWriteEeprom = false;

	if (changeOn != on)
	{
		println (LEVEL_DEBUG, false);
		print (LEVEL_DEBUG, "\"On\" changed from ");
		print (LEVEL_DEBUG, changeOn ? "True" : "False", false);
		print (LEVEL_DEBUG, " to ", false);
		println (LEVEL_DEBUG, on ? "True" : "False", false);
		changeOn     = on;
		flagSendInfo = true;
	}

	if (changeRgb != rgb[MODE_DEFAULT])
	{
		println (LEVEL_DEBUG, false);
		print (LEVEL_DEBUG, "\"RGB\" of DEFAULT changed from ");
		print (LEVEL_DEBUG, changeRgb, HEX, false);
		print (LEVEL_DEBUG, " to ", false);
		println (LEVEL_DEBUG, rgb[MODE_DEFAULT], HEX, false);
		changeRgb       = rgb[MODE_DEFAULT];
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}


	for (int i = MODE_MIN; i < N_MODE; i++)
		if (changePower[i] != power[i])
		{
			println (LEVEL_DEBUG, false);
			print (LEVEL_DEBUG, "\"Power\" of " + modeName (i, CAPS_NONE) + " changed from ");
			print (LEVEL_DEBUG, (int) changePower[i], DEC, false);
			print (LEVEL_DEBUG, " to ", false);
			println (LEVEL_DEBUG, (int) power[i], DEC, false);
			changePower[i] = power[i];
			flagSendInfo   = true;
			if (i != MODE_WAKEUP)
				flagWriteEeprom = true;
		}


	for (int i = MODE_MIN; i < N_MODE; i++)
		if (changeSpeed[i] != speed[i])
		{
			println (LEVEL_DEBUG, false);
			print (LEVEL_DEBUG, "\"Speed\" of " + modeName (i, CAPS_NONE) + " changed from ");
			print (LEVEL_DEBUG, changeSpeed[i], DEC, false);
			print (LEVEL_DEBUG, " to ", false);
			println (LEVEL_DEBUG, speed[i], DEC, false);
			changeSpeed[i]  = speed[i];
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}

	if (changeMode != mode)
	{
		println (LEVEL_DEBUG, false);
		print (LEVEL_DEBUG, "\"Mode\" changed from ");
		print (LEVEL_DEBUG, modeName (changeMode, CAPS_NONE), false);
		print (LEVEL_DEBUG, " to ", false);
		println (LEVEL_DEBUG, modeName (mode, CAPS_NONE), false);
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
	println (LEVEL_DEBUG, false);
	println (LEVEL_DEBUG, "Sending variables infos to the ESP8266");
	print (LEVEL_DEBUG, "(");

	Serial1.print ("ONF");
	Serial1.print (on ? 1 : 0, DEC);
	Serial1.print ("z");
	print (LEVEL_DEBUG, "ONF", false);
	print (LEVEL_DEBUG, on ? 1 : 0, DEC, false);
	print (LEVEL_DEBUG, "z ", false);

	Serial1.print ("MOD");
	Serial1.print (mode, DEC);
	Serial1.print ("z");
	print (LEVEL_DEBUG, "MOD", false);
	print (LEVEL_DEBUG, mode, DEC, false);
	print (LEVEL_DEBUG, "z ", false);

	Serial1.print ("RGB");
	Serial1.print (rgb[MODE_DEFAULT], HEX);
	Serial1.print ("z");
	print (LEVEL_DEBUG, "RGB", false);
	print (LEVEL_DEBUG, rgb[MODE_DEFAULT], HEX, false);
	print (LEVEL_DEBUG, "z ", false);

	for (int i = MODE_MIN; i < N_MODE; i++)
	{
		Serial1.print ("POW");
		Serial1.print (i, DEC);
		Serial1.print ((int) power[i], DEC);
		Serial1.print ("z");
		print (LEVEL_DEBUG, "POW", false);
		print (LEVEL_DEBUG, i, DEC, false);
		print (LEVEL_DEBUG, (int) power[i], DEC, false);
		print (LEVEL_DEBUG, "z ", false);
	}

	for (int i = MODE_MIN; i < N_MODE; i++)
	{
		Serial1.print ("SPE");
		Serial1.print (i, DEC);
		Serial1.print ((((long) speed[i] - (MIN_SPEED[i] - SEEKBAR_MIN)) * (SEEKBAR_MAX - SEEKBAR_MIN)) / (MAX_SPEED[i] - MIN_SPEED[i]), DEC);
		Serial1.print ("z");
		print (LEVEL_DEBUG, "SPE", false);
		print (LEVEL_DEBUG, i, DEC, false);
		print (LEVEL_DEBUG, (((long) speed[i] - (MIN_SPEED[i] - SEEKBAR_MIN)) * (SEEKBAR_MAX - SEEKBAR_MIN)) / (MAX_SPEED[i] - MIN_SPEED[i]), DEC, false);
		print (LEVEL_DEBUG, "z ", false);
	}

	print (LEVEL_DEBUG, ")", false);
} // sendInfo
