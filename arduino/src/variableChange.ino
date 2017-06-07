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

	if (changeRgb != rgb)
	{
		printlnNoPrefix();
		print ("\"RGB\" changed from ");
		printNoPrefix (changeRgb, HEX);
		printNoPrefix (" to ");
		printlnNoPrefix (rgb, HEX);
		changeRgb = rgb;
		sendInfo();
	}

	if (changePower != power)
	{
		printlnNoPrefix();
		print ("\"Power\" changed from ");
		printNoPrefix ((int) changePower, DEC);
		printNoPrefix (" to ");
		printlnNoPrefix ((int) power, DEC);
		changePower = power;
		sendInfo();
	}

	if (changeMode != mode)
	{
		printlnNoPrefix();
		print ("\"Power\" changed from ");
		printlnNoPrefix (modeName (changeMode));
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
	changeRgb   = DEFAULT_COLOR;
	changeMode  = MODE_DEFAULT;
	changePower = DEFAULT_POWER;
}

void sendInfo ()
{
	println ("Sending variables infos to the ESP8266");

	Serial1.print ("RGB");
	Serial1.print (rgb, HEX);
	Serial1.print ("z");

	Serial1.print ("ONF");
	Serial1.print (on ? 1 : 0, DEC);
	Serial1.print ("z");

	Serial1.print ("POW");
	Serial1.print ((int) power, DEC);
	Serial1.print ("z");

	Serial1.print ("MOD");
	Serial1.print (mode, DEC);
	Serial1.print ("z");

	printlnNoPrefix();
}
