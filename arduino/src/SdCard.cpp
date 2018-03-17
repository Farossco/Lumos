#include <stdlib.h>
#include <Time.h>
#include "SdCard.h"
#include "Logger.h"

SdCard::SdCard()
{
	pin          = 0;
	enabled      = false;
	creationDate = -1;
}

void SdCard::init (int pin)
{
	// Si le log SD est décactivé, on arrête là
	if (!SD_LOG_ENABLED)
		return;

	// Si la date de création du fichier est celle d'aujourd'hui, on ne recrée pas de fichier
	if (creationDate == year() * 32 * 13 + month() * 32 + day())
		return;

	this->pin = pin;

	Log.info ("Initializing SD card... ");

	enabled = false;

	closeFile(); // Fermeture du fichier s'il est ouvert

	SD.end(); // Fermeture de la connexion à la carte SD

	// Ouverture d'une connexion à la carte SD
	if (SD.begin (pin))
	{
		Log.infonp ("Done." dendl);
	}
	else // En cas d'échec de la connexion
	{
		Log.infonp ("Failed!" dendl);
		Log.error ("SD Initialisation failed! No logging for this session..." dendl);
		return;
	}

	// Création du fichier log
	logFileAvailable = createLogFile();

	if (logFileAvailable) // Si la création du fichier n'a pas échouée
	{
		// Printing it as lowest level so it is printed for any debug level
		// It's not a problem since it's not gonna print the prefix
		Log.errornp ("------------------------------------------------------------------------------------------------------------" endl);
		Log.errornp ("----------------------------------------------- SD log Start -----------------------------------------------" endl);
		Log.errornp ("------------------------------------------------------------------------------------------------------------" dendl);

		enabled = true;
	}

	creationDate = year() * 32 * 13 + month() * 32 + day();
} // SdCard::init

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
		Log.trace ("File opened" dendl);
	}

	fileLastOpened = millis();
}

void SdCard::closeFile ()
{
	if (!enabled || fileIsClosed())
		return;

	if (millis() - fileLastOpened > FILE_CLOSE_TIME * 1000)
	{
		Log.trace ("File closed" dendl);
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
