#include "esp8266.h"

void printPrefix ()
{
	if (DEBUG_ENABLED)
	{
		Serial.print ("[");
		debugClockDisplayNoPrefix();
		Serial.print ("] [DEBUG] ");
	}
}

// Utility for digital clock display: prints leading 0
void printDebugDigits (int digits)
{
	if (DEBUG_ENABLED)
	{
		if (digits < 10)
			Serial.print (0);
		Serial.print (digits);
	}
}

// Digital clock display of the time
void debugClockDisplay ()
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		printDebugDigits (day());
		Serial.print ("/");
		printDebugDigits (month());
		Serial.print ("/");
		Serial.print (year());

		Serial.print (" ");

		printDebugDigits (hour());
		Serial.print (":");
		printDebugDigits (minute());
		Serial.print (":");
		printDebugDigits (second());
	}
}

void debugClockDisplayNoPrefix ()
{
	if (DEBUG_ENABLED)
	{
		printDebugDigits (day());
		Serial.print ("/");
		printDebugDigits (month());
		Serial.print ("/");
		Serial.print (year());

		Serial.print (" ");

		printDebugDigits (hour());
		Serial.print (":");
		printDebugDigits (minute());
		Serial.print (":");
		printDebugDigits (second());
	}
}

size_t print (const __FlashStringHelper * message)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.print (message);
	}

	return -1;
}

size_t print (const String & message)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.print (message);
	}

	return -1;
}

size_t print (const char * message)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.print (message);
	}

	return -1;
}

size_t print (unsigned char message, int base)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.print (message, base);
	}

	return -1;
}

size_t print (int message, int base)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.print (message, base);
	}

	return -1;
}

size_t print (unsigned int message, int base)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.print (message, base);
	}

	return -1;
}

size_t print (long message, int base)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.print (message, base);
	}

	return -1;
}

size_t print (unsigned long message, int base)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.print (message, base);
	}

	return -1;
}

size_t print (double message, int base)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.print (message, base);
	}

	return -1;
}

size_t print (const Printable & message)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.print (message);
	}

	return -1;
}

size_t printNoPrefix (const __FlashStringHelper * message)
{
	if (DEBUG_ENABLED)
		return Serial.print (message);

	return -1;
}

size_t printNoPrefix (const String & message)
{
	if (DEBUG_ENABLED)
		return Serial.print (message);

	return -1;
}

size_t printNoPrefix (const char * message)
{
	if (DEBUG_ENABLED)
		return Serial.print (message);

	return -1;
}

size_t printNoPrefix (unsigned char message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.print (message, base);

	return -1;
}

size_t printNoPrefix (int message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.print (message, base);

	return -1;
}

size_t printNoPrefix (unsigned int message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.print (message, base);

	return -1;
}

size_t printNoPrefix (long message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.print (message, base);

	return -1;
}

size_t printNoPrefix (unsigned long message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.print (message, base);

	return -1;
}

size_t printNoPrefix (double message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.print (message, base);

	return -1;
}

size_t printNoPrefix (const Printable & message)
{
	if (DEBUG_ENABLED)
		return Serial.print (message);

	return -1;
}

size_t println ()
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.println();
	}

	return -1;
}

size_t println (const __FlashStringHelper * message)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.println (message);
	}

	return -1;
}

size_t println (const String & message)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.println (message);
	}

	return -1;
}

size_t println (const char * message)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.println (message);
	}

	return -1;
}

size_t println (unsigned char message, int base)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.println (message, base);
	}

	return -1;
}

size_t println (int message, int base)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.println (message, base);
	}

	return -1;
}

size_t println (unsigned int message, int base)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.println (message, base);
	}

	return -1;
}

size_t println (long message, int base)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.println (message, base);
	}

	return -1;
}

size_t println (unsigned long message, int base)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.println (message, base);
	}

	return -1;
}

size_t println (double message, int base)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.println (message, base);
	}

	return -1;
}

size_t println (const Printable & message)
{
	if (DEBUG_ENABLED)
	{
		printPrefix();
		return Serial.println (message);
	}

	return -1;
}

size_t printlnNoPrefix ()
{
	if (DEBUG_ENABLED)
		return Serial.println();

	return -1;
}

size_t printlnNoPrefix (const __FlashStringHelper * message)
{
	if (DEBUG_ENABLED)
		return Serial.println (message);

	return -1;
}

size_t printlnNoPrefix (const String & message)
{
	if (DEBUG_ENABLED)
		return Serial.println (message);

	return -1;
}

size_t printlnNoPrefix (const char * message)
{
	if (DEBUG_ENABLED)
		return Serial.println (message);

	return -1;
}

size_t printlnNoPrefix (unsigned char message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.println (message, base);

	return -1;
}

size_t printlnNoPrefix (int message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.println (message, base);

	return -1;
}

size_t printlnNoPrefix (unsigned int message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.println (message, base);

	return -1;
}

size_t printlnNoPrefix (long message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.println (message, base);

	return -1;
}

size_t printlnNoPrefix (unsigned long message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.println (message, base);

	return -1;
}

size_t printlnNoPrefix (double message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.println (message, base);

	return -1;
}

size_t printlnNoPrefix (const Printable & message)
{
	if (DEBUG_ENABLED)
		return Serial.println (message);

	return -1;
}
