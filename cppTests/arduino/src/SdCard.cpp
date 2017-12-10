#include <stdlib.h>
#include <Time.h>
#include "SdCard.h"
#include "Logger.h"

SdCard::SdCard()
{
	pin = 0;
}

void SdCard::begin (int pin)
{
	this->pin = pin;

	Log.info ("Initializing SD card... ");

	if (SD.begin (pin))
	{
		Log.infonp ("Done." dendl);
	}
	else
	{
		Log.infonp ("Failed!" dendl);
		Log.error ("SD Initialisation failed! No logging for this session..." dendl);
		return;
	}

	logFileAvailable = createLogFile();

	if (logFileAvailable)
	{
		// Printing it as lowest level so it is printed for any debug level
		// It's not a problem since it's not gonna print the prefix
		Log.errornp ("------------------------------------------------------------------------------------------------------------" endl);
		Log.errornp ("----------------------------------------------- SD log Start -----------------------------------------------" endl);
		Log.errornp ("------------------------------------------------------------------------------------------------------------" dendl);
	}
}

boolean SdCard::createLogFile ()
{
	char logFileName[13];

	getLogFileName (logFileName);

	const char * loadingCreating = SD.exists (logFileName) ? "Loading" : "Creating";

	Log.info ("%s log file... ", loadingCreating);

	logFile = SD.open (logFileName, FILE_WRITE);

	if (strlen (logFile.name()) < 1)
	{
		Log.infonp ("Failed!" dendl);
		Log.error ("%s log file failed! No logging for this session..." dendl, loadingCreating);
		return false;
	}

	Log.infonp ("Done.");
	Log.tracenp (" (%s)", logFile.name());
	Log.infonp (dendl);

	logFile.close();

	return true;
}

// A 13-char buffer is necessary
void SdCard::getLogFileName (char * buf)
{
	sprintf (buf, "%d%.2d%.2d.log", year(), month(), day());
}

SdCard sd = SdCard();
