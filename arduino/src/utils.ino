#include "arduino.h"

String modeName (int mode, int caps)
{
	switch (mode)
	{
		case MODE_DEFAULT:
			return caps == 0 ? "default" : caps == 1 ? "Default" : "DEFAULT";

		case MODE_FLASH:
			return caps == 0 ? "flash" : caps == 1 ? "Flash" : "FLASH";

		case MODE_STROBE:
			return caps == 0 ? "strobe" : caps == 1 ? "Strobe" : "STROBE";

		case MODE_FADE:
			return caps == 0 ? "fade" : caps == 1 ? "Fade" : "FADE";

		case MODE_SMOOTH:
			return caps == 0 ? "smooth" : caps == 1 ? "Smooth" : "SMOOTH";

		case MODE_WAKEUP:
			return caps == 0 ? "wake up" : caps == 1 ? "Wake up" : "WAKE UP";

		default:
			return caps == 0 ? "unknown" : caps == 1 ? "Unknown" : "UNKNOWN";
	}
}

String infoTypeName (int infoType, boolean shortened)
{
	switch (infoType)
	{
		case TYPE_TIM:
			return shortened ? "TIM" : "Time";

		case TYPE_RGB:
			return "RGB";

		case TYPE_ONF:
			return shortened ? "ONF" : "On";

		case TYPE_POW:
			return shortened ? "POW" : "Power";

		case TYPE_MOD:
			return shortened ? "MOD" : "Mode";

		case TYPE_PRT:
			return shortened ? "PRT" : "Prayer time";

		case TYPE_SPE:
			return shortened ? "SPE" : "Speed";

		default:
			return shortened ? "UNK" : "Unknown";
	}
}

String errorTypeName (int infoType, boolean shortened)
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

		case ERR_UKP:
			return shortened ? "Unknown prayer" : "Error: Unknown prayer";

		default:
			return "Unknown error";
	}
}

String debugLevelName(int debugLevel)
{
	switch (debugLevel)
	{
		case LEVEL_DEBUG:
			return "DEBUG";

		case LEVEL_INFO:
			return "INFO";

		case LEVEL_ERROR:
			return "ERROR";

		default:
			return "?????";
	}
}

String debugLevelSpace(int debugLevel)
{
	switch (debugLevel)
	{
		case LEVEL_DEBUG:
			return " ";

		case LEVEL_INFO:
			return "  ";

		case LEVEL_ERROR:
			return " ";

		default:
			return " ";
	}
}

// Digital clock display of the time
void digitalClockDisplay (int debugLevel)
{
	printDigits (debugLevel, day());
	printNoPrefix (debugLevel, "/");
	printDigits (debugLevel, month());
	printNoPrefix (debugLevel, "/");
	printNoPrefix (debugLevel, year(), DEC);

	printNoPrefix (debugLevel, " ");

	printDigits (debugLevel, hour());
	printNoPrefix (debugLevel, ":");
	printDigits (debugLevel, minute());
	printNoPrefix (debugLevel, ":");
	printDigits (debugLevel, second());

	printlnNoPrefix(debugLevel);
}

// Utility for digital clock display: prints preceding colon and leading 0
void printDigits (int debugLevel, int digits)
{
	if (digits < 10)
		printNoPrefix (debugLevel, 0, DEC);

	printNoPrefix (debugLevel, digits, DEC);
}

void softwareReset () // Just in case
{
	asm volatile ("  jmp 0");
}
