#include <stdlib.h>
#include <Time.h>
#include "SdCard.h"
#include "Logger.h"

SdCard::SdCard()
{
	pin     = 0;
	enabled = false;
}

void SdCard::init (int pin)
{
	if (!SD_LOG_ENABLED)
		return;

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

		enabled = true;
	}
}

File * SdCard::getFile ()
{
	return &logFile;
}

boolean SdCard::fileIsOpened ()
{
	return fileOpened;
}

boolean SdCard::fileIsClosed ()
{
	return !fileOpened;
}

void SdCard::openFile ()
{
	if (!enabled)
		return;

	if (fileIsClosed())
	{
		logFile    = SD.open (sdFileName, FILE_WRITE);
		fileOpened = true;
		Log.verbose ("File opened" dendl);
	}

	fileLastOpened = millis();
}

void SdCard::closeFile ()
{
	if (!enabled || fileIsClosed())
		return;

	if (millis() - fileLastOpened > FILE_CLOSE_TIME * 1000)
	{
		Log.verbose ("File closed" dendl);
		logFile.close();
		fileOpened = false;
	}
}

boolean SdCard::isEnabled ()
{
	return enabled;
}

boolean SdCard::createLogFile ()
{
	getLogFileName (sdFileName);

	const char * loadingCreating = SD.exists (sdFileName) ? "Loading" : "Creating";

	Log.info ("%s log file... ", loadingCreating);

	logFile = SD.open (sdFileName, FILE_WRITE);

	if (strlen (logFile.name()) < 1)
	{
		Log.infonp ("Failed!" dendl);
		Log.error ("%s log file failed! No logging for this session..." dendl, loadingCreating);
		return false;
	}

	Log.infonp ("Done. (%s)" dendl, logFile.name());

	logFile.close();

	return true;
}

// A 13-char buffer is necessary
void SdCard::getLogFileName (char * buf)
{
	sprintf (buf, "%d%.2d%.2d.log", year(), month(), day());
}

SdCard sd = SdCard();
