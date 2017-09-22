#include "arduino.h"

void printPrefix(int debugLevel)
{
	Serial.print("[");
	debugPrintDigits(day());
	Serial.print("/");
	debugPrintDigits(month());
	Serial.print("/");
	Serial.print(year());

	Serial.print(" ");

	debugPrintDigits(hour());
	Serial.print(":");
	debugPrintDigits(minute());
	Serial.print(":");
	debugPrintDigits(second());
	Serial.print("] [" + debugLevelName(debugLevel) + "]");
	Serial.print(debugLevelSpace(debugLevel));
}

void printSdPrefix(int debugLevel)
{
	logFile.print("[");
	sdPrintDigits(day());
	logFile.print("/");
	sdPrintDigits(month());
	logFile.print("/");
	logFile.print(year());

	logFile.print(" ");

	sdPrintDigits(hour());
	logFile.print(":");
	sdPrintDigits(minute());
	logFile.print(":");
	sdPrintDigits(second());
	logFile.print("] [" + debugLevelName(debugLevel) + "] ");
	logFile.print(debugLevelSpace(debugLevel));
}

// Utility for digital clock display: prints leading 0
void debugPrintDigits(int digits)
{
	if (digits < 10)
		Serial.print(0);
	Serial.print(digits);
}

void sdPrintDigits(int digits)
{
	if (digits < 10)
		logFile.print(0);
	logFile.print(digits);
}

// ------------------------------------------------------------------------------------- //
// ---------------------------------- Print functions ---------------------------------- //
// ------------------------------------------------------------------------------------- //

void print(int debugLevel, const __FlashStringHelper *message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.print(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.print(message);
	}
}

void print(int debugLevel, const String &message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.print(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.print(message);
	}
}

void print(int debugLevel, const char *message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.print(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.print(message);
	}
}

void print(int debugLevel, unsigned char message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.print(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.print(message, base);
	}
}

void print(int debugLevel, int message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.print(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.print(message, base);
	}
}

void print(int debugLevel, unsigned int message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.print(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.print(message, base);
	}
}

void print(int debugLevel, long message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.print(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.print(message, base);
	}
}

void print(int debugLevel, unsigned long message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.print(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.print(message, base);
	}
}

void print(int debugLevel, double message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.print(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.print(message, base);
	}
}

void print(int debugLevel, const Printable &message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.print(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.print(message);
	}
}

void printNoPrefix(int debugLevel, const __FlashStringHelper *message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.print(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.print(message);
	}
}

void printNoPrefix(int debugLevel, const String &message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.print(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.print(message);
	}
}

void printNoPrefix(int debugLevel, const char *message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.print(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.print(message);
	}
}

void printNoPrefix(int debugLevel, unsigned char message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.print(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.print(message, base);
	}
}

void printNoPrefix(int debugLevel, int message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.print(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.print(message, base);
	}
}

void printNoPrefix(int debugLevel, unsigned int message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.print(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.print(message, base);
	}
}

void printNoPrefix(int debugLevel, long message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.print(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.print(message, base);
	}
}

void printNoPrefix(int debugLevel, unsigned long message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.print(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.print(message, base);
	}
}

void printNoPrefix(int debugLevel, double message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.print(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.print(message, base);
	}
}

void printNoPrefix(int debugLevel, const Printable &message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.print(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.print(message);
	}
}

void println(int debugLevel)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.println();
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.println();
	}
}

void println(int debugLevel, const __FlashStringHelper *message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.println(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.println(message);
	}
}

void println(int debugLevel, const String &message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.println(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.println(message);
	}
}

void println(int debugLevel, const char *message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.println(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.println(message);
	}
}

void println(int debugLevel, unsigned char message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.println(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.println(message, base);
	}
}

void println(int debugLevel, int message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.println(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.println(message, base);
	}
}

void println(int debugLevel, unsigned int message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.println(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.println(message, base);
	}
}

void println(int debugLevel, long message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.println(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.println(message, base);
	}
}

void println(int debugLevel, unsigned long message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.println(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.println(message, base);
	}
}

void println(int debugLevel, double message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.println(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.println(message, base);
	}
}

void println(int debugLevel, const Printable &message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		printSdPrefix(debugLevel);
		logFile.println(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		printPrefix(debugLevel);
		Serial.println(message);
	}
}

void printlnNoPrefix(int debugLevel)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.println();
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.println();
	}
}

void printlnNoPrefix(int debugLevel, const __FlashStringHelper *message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.println(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.println(message);
	}
}

void printlnNoPrefix(int debugLevel, const String &message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.println(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.println(message);
	}
}

void printlnNoPrefix(int debugLevel, const char *message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.println(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.println(message);
	}
}

void printlnNoPrefix(int debugLevel, unsigned char message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.println(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.println(message, base);
	}
}

void printlnNoPrefix(int debugLevel, int message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.println(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.println(message, base);
	}
}

void printlnNoPrefix(int debugLevel, unsigned int message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.println(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.println(message, base);
	}
}

void printlnNoPrefix(int debugLevel, long message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.println(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.println(message, base);
	}
}

void printlnNoPrefix(int debugLevel, unsigned long message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.println(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.println(message, base);
	}
}

void printlnNoPrefix(int debugLevel, double message, int base)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.println(message, base);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.println(message, base);
	}
}

void printlnNoPrefix(int debugLevel, const Printable &message)
{
	if (SD_LOG_ENABLED && logFileAvailable && debugLevel <= SD_LOG_LEVEL)
	{
		logFile = SD.open(sdFileName, FILE_WRITE);
		logFile.println(message);
		logFile.close();
	}

	if (SERIAL_LOG_ENABLED && debugLevel <= SERIAL_LOG_LEVEL)
	{
		Serial.println(message);
	}
}
