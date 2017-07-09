boolean changeOn;
unsigned long changeRgb;
float changePower[MODE_MAX + 1];
int changeSpeed[MODE_MAX + 1];
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
		print ("\"RGB\" of DEFAULT changed from ");
		printNoPrefix (changeRgb, HEX);
		printNoPrefix (" to ");
		printlnNoPrefix (rgb[MODE_DEFAULT], HEX);
		changeRgb = rgb[MODE_DEFAULT];
		sendInfo();
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
	changeOn   = false;
	changeRgb  = defaultRgb[MODE_DEFAULT];
	changeMode = MODE_DEFAULT;
	for (int i = 0; i <= MODE_MAX; i++)
		changePower[i] = defaultPower[i];
	for (int i = 0; i <= MODE_MAX; i++)
		changeSpeed[i] = defaultSpeed[i];
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

	printlnNoPrefix();
}
