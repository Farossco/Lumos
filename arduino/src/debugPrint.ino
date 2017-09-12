#include "arduino.h"

void printPrefix ()
{
	if (DEBUG_ENABLED)
	{
		Serial.print ("[");
		debugClockDisplay();
		Serial.print ("] [DEBUG] ");
	}
}

void printSdPrefix ()
{
	if (DEBUG_ENABLED)
	{
		logFile.print ("[");
		sdClockDisplay();
		logFile.print ("] [DEBUG] ");
	}
}

// Utility for digital clock display: prints leading 0
void debugPrintDigits (int digits)
{
	if (DEBUG_ENABLED)
	{
		if (digits < 10)
			Serial.print (0);
		Serial.print (digits);
	}
}

void sdPrintDigits (int digits)
{
	if (DEBUG_ENABLED)
	{
		if (digits < 10)
			logFile.print (0);
		logFile.print (digits);
	}
}

// Digital clock display of the time
void debugClockDisplay ()
{
	if (DEBUG_ENABLED)
	{
		debugPrintDigits (day());
		Serial.print ("/");
		debugPrintDigits (month());
		Serial.print ("/");
		Serial.print (year());

		Serial.print (" ");

		debugPrintDigits (hour());
		Serial.print (":");
		debugPrintDigits (minute());
		Serial.print (":");
		debugPrintDigits (second());
	}
}

void sdClockDisplay ()
{
	if (DEBUG_ENABLED)
	{
		sdPrintDigits (day());
		logFile.print ("/");
		sdPrintDigits (month());
		logFile.print ("/");
		logFile.print (year());

		logFile.print (" ");

		sdPrintDigits (hour());
		logFile.print (":");
		sdPrintDigits (minute());
		logFile.print (":");
		sdPrintDigits (second());
	}
}

size_t print (const __FlashStringHelper * message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.print (message);
			logFile.close();
		}

		printPrefix();
		return Serial.print (message);
	}

	return -1;
}

size_t print (const String & message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.print (message);
			logFile.close();
		}

		printPrefix();
		return Serial.print (message);
	}

	return -1;
}

size_t print (const char * message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.print (message);
			logFile.close();
		}

		printPrefix();
		return Serial.print (message);
	}

	return -1;
}

size_t print (unsigned char message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.print (message, base);
			logFile.close();
		}

		printPrefix();
		return Serial.print (message, base);
	}

	return -1;
}

size_t print (int message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.print (message, base);
			logFile.close();
		}

		printPrefix();
		return Serial.print (message, base);
	}

	return -1;
}

size_t print (unsigned int message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.print (message, base);
			logFile.close();
		}

		printPrefix();
		return Serial.print (message, base);
	}

	return -1;
}

size_t print (long message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.print (message, base);
			logFile.close();
		}

		printPrefix();
		return Serial.print (message, base);
	}

	return -1;
}

size_t print (unsigned long message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.print (message, base);
			logFile.close();
		}

		printPrefix();
		return Serial.print (message, base);
	}

	return -1;
}

size_t print (double message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.print (message, base);
			logFile.close();
		}

		printPrefix();
		return Serial.print (message, base);
	}

	return -1;
}

size_t print (const Printable & message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.print (message);
			logFile.close();
		}

		printPrefix();
		return Serial.print (message);
	}

	return -1;
}

size_t printNoPrefix (const __FlashStringHelper * message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.print (message);
			logFile.close();
		}

		return Serial.print (message);
	}

	return -1;
}

size_t printNoPrefix (const String & message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.print (message);
			logFile.close();
		}

		return Serial.print (message);
	}

	return -1;
}

size_t printNoPrefix (const char * message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.print (message);
			logFile.close();
		}

		return Serial.print (message);
	}

	return -1;
}

size_t printNoPrefix (unsigned char message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.print (message, base);
			logFile.close();
		}

		return Serial.print (message, base);
	}

	return -1;
}

size_t printNoPrefix (int message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.print (message, base);
			logFile.close();
		}

		return Serial.print (message, base);
	}

	return -1;
}

size_t printNoPrefix (unsigned int message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.print (message, base);
			logFile.close();
		}

		return Serial.print (message, base);
	}

	return -1;
}

size_t printNoPrefix (long message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.print (message, base);
			logFile.close();
		}

		return Serial.print (message, base);
	}

	return -1;
}

size_t printNoPrefix (unsigned long message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.print (message, base);
			logFile.close();
		}

		return Serial.print (message, base);
	}

	return -1;
}

size_t printNoPrefix (double message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.print (message, base);
			logFile.close();
		}

		return Serial.print (message, base);
	}

	return -1;
}

size_t printNoPrefix (const Printable & message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.print (message);
			logFile.close();
		}

		return Serial.print (message);
	}

	return -1;
}

size_t println ()
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.println();
			logFile.close();
		}
		printPrefix();
		return Serial.println();
	}

	return -1;
}

size_t println (const __FlashStringHelper * message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.println (message);
			logFile.close();
		}
		printPrefix();
		return Serial.println (message);
	}

	return -1;
}

size_t println (const String & message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.println (message);
			logFile.close();
		}
		printPrefix();
		return Serial.println (message);
	}

	return -1;
}

size_t println (const char * message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.println (message);
			logFile.close();
		}
		printPrefix();
		return Serial.println (message);
	}

	return -1;
}

size_t println (unsigned char message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.println (message, base);
			logFile.close();
		}
		printPrefix();
		return Serial.println (message, base);
	}

	return -1;
}

size_t println (int message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.println (message, base);
			logFile.close();
		}
		printPrefix();
		return Serial.println (message, base);
	}

	return -1;
}

size_t println (unsigned int message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.println (message, base);
			logFile.close();
		}
		printPrefix();
		return Serial.println (message, base);
	}

	return -1;
}

size_t println (long message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.println (message, base);
			logFile.close();
		}
		printPrefix();
		return Serial.println (message, base);
	}

	return -1;
}

size_t println (unsigned long message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.println (message, base);
			logFile.close();
		}
		printPrefix();
		return Serial.println (message, base);
	}

	return -1;
}

size_t println (double message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.println (message, base);
			logFile.close();
		}
		printPrefix();
		return Serial.println (message, base);
	}

	return -1;
}

size_t println (const Printable & message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			printSdPrefix();
			logFile.println (message);
			logFile.close();
		}
		printPrefix();
		return Serial.println (message);
	}

	return -1;
}

size_t printlnNoPrefix ()
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.println();
			logFile.close();
		}

		return Serial.println();
	}
	
	return -1;
}

size_t printlnNoPrefix (const __FlashStringHelper * message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.println(message);
			logFile.close();
		}

		return Serial.println(message);
	}
	
	return -1;
}

size_t printlnNoPrefix (const String & message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.println(message);
			logFile.close();
		}

		return Serial.println(message);
	}
	
	return -1;
}

size_t printlnNoPrefix (const char * message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.println(message);
			logFile.close();
		}

		return Serial.println(message);
	}
	
	return -1;
}

size_t printlnNoPrefix (unsigned char message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.println(message, base);
			logFile.close();
		}

		return Serial.println(message, base);
	}
	
	return -1;
}

size_t printlnNoPrefix (int message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.println(message, base);
			logFile.close();
		}

		return Serial.println(message, base);
	}
	
	return -1;
}

size_t printlnNoPrefix (unsigned int message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.println(message, base);
			logFile.close();
		}

		return Serial.println(message, base);
	}
	
	return -1;
}

size_t printlnNoPrefix (long message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.println(message, base);
			logFile.close();
		}

		return Serial.println(message, base);
	}
	
	return -1;
}

size_t printlnNoPrefix (unsigned long message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.println(message, base);
			logFile.close();
		}

		return Serial.println(message, base);
	}
	
	return -1;
}

size_t printlnNoPrefix (double message, int base)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.println(message, base);
			logFile.close();
		}

		return Serial.println(message, base);
	}
	
	return -1;
}

size_t printlnNoPrefix (const Printable & message)
{
	if (DEBUG_ENABLED)
	{
		if (logFileAvailable)
		{
			logFile = SD.open (sdFileName, FILE_WRITE);
			logFile.println(message);
			logFile.close();
		}

		return Serial.println(message);
	}
	
	return -1;
}
