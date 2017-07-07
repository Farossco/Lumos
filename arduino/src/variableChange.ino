boolean changeOn;
unsigned long changeRgb;
float changePower;
unsigned char changeMode;

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
		print ("\"RGB\" changed from ");
		printNoPrefix (changeRgb, HEX);
		printNoPrefix (" to ");
		printlnNoPrefix (rgb[MODE_DEFAULT], HEX);
		changeRgb = rgb[MODE_DEFAULT];
		sendInfo();
	}

	if (changePower != power[MODE_DEFAULT])
	{
		printlnNoPrefix();
		print ("\"Power\" changed from ");
		printNoPrefix ((int) changePower, DEC);
		printNoPrefix (" to ");
		printlnNoPrefix ((int) power[MODE_DEFAULT], DEC);
		changePower = power[MODE_DEFAULT];
		sendInfo();
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
	}
} // testVariableChange

void initVariableChange ()
{
	// Initializing to default values
	changeOn    = false;
	changeRgb   = defaultRgb[MODE_DEFAULT];
	changeMode  = MODE_DEFAULT;
	changePower = defaultPower[MODE_DEFAULT];
}

void sendInfo ()
{
	println ("Sending variables infos to the ESP8266");

	Serial1.print ("ONF");
	Serial1.print (on ? 1 : 0, DEC);
	Serial1.print ("z");

	Serial1.print ("MOD");
	Serial1.print (mode, DEC);
	Serial1.print ("z");

	for (int i = 0; i <= MODE_MAX; i++)
	{
		Serial1.print ("RGB");
		Serial1.print (i);
		Serial1.print (rgb[i], HEX);
		Serial1.print ("z");

		Serial1.print ("POW");
		Serial1.print (i);
		Serial1.print ((int) power[i], DEC);
		Serial1.print ("z");

		Serial1.print ("SPE");
		Serial1.print (i);
		Serial1.print (speed[i], DEC);
		Serial1.print ("z");
	}

	printlnNoPrefix();
}
