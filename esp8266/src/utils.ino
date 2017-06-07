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
