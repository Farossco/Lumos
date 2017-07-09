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

String infoTypeName (int infoType, boolean shortened)
{
	switch (infoType)
	{
		case TYPE_RGB:
			return "RGB";

		case TYPE_ONF:
			return shortened ? "ONF" : "On";

		case TYPE_POW:
			return shortened ? "POW" : "Power";

		case TYPE_MOD:
			return shortened ? "MOD" : "Mode";

		case TYPE_SPE:
			return shortened ? "SPE" : "Speed";

		default:
			return shortened ? "UNK" : "Unknown";
	}
}

String ErrorTypeName (int infoType, boolean shortened)
{
	switch (infoType)
	{
		case ERR_NOE:
			return "No error";

		case ERR_OOB:
			return shortened ? "Out of bounds" : "Error: Value is out of bounds";

		case ERR_UKM:
			return shortened ? "Unknowm mode" : "Error: Unknowm mode";

		case ERR_UKR:
			return shortened ? "Unknown request type" : "Error: Unknown request type";

		default:
			return "Unknown error";
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
