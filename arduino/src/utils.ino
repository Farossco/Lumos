String modeName (int mode)
{
	switch (mode)
	{
		case MODE_DEFAULT:
			return "DEFAULT";

		case MODE_FLASH:
			return "FLASH";

		case MODE_STROBE:
			return "STROBE";

		case MODE_FADE:
			return "FADE";

		case MODE_SMOOTH:
			return "SMOOTH";

		case MODE_WAKEUP:
			return "WAKE UP";

		default:
			return "UNKNOWN";
	}
}

// Digital clock display of the time
void digitalClockDisplay ()
{
	printDigits (day());
	printNoPrefix ("/");
	printDigits (month());
	printNoPrefix ("/");
	printNoPrefix (year(), DEC);

	printNoPrefix (" ");

	printDigits (hour());
	printNoPrefix (":");
	printDigits (minute());
	printNoPrefix (":");
	printDigits (second());

	printlnNoPrefix();
}

// Utility for digital clock display: prints preceding colon and leading 0
void printDigits (int digits)
{
	if (digits < 10)
		printNoPrefix (0, DEC);

	printNoPrefix (digits, DEC);
}
