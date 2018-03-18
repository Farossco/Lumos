#include <stdlib.h>
#include <Time.h>
#include "SdCard.h"
#include "Logger.h"

SdCard::SdCard()
{
	enabled          = false;
	logFileAvailable = false;
	fileOpened       = false;
	cardOpened       = false;
	cardPresent      = false;
	fileLastOpened   = 0;
	cardLastChecked  = 0;
}

void SdCard::init ()
{
	// Si le log SD est désactivé, on arrête là
	if (!SD_LOG_ENABLED)
		return;

	enabled = false;

	closeFile(); // Fermeture du fichier s'il est ouvert

	SD.end(); // Fermeture de la connexion à la carte SD
	cardOpened = false;

	if (cardIsDetected())
	{
		cardPresent = true;
		Log.info ("SD card detected, initializing... ");
	}
	else
	{
		cardPresent = false;
		Log.warning ("SD card undetected, no SD logging..." dendl);
		return;
	}

	// Ouverture d'une connexion à la carte SD
	if (SD.begin (PIN_SD_CS))
	{
		cardOpened = true;
		Log.infonp ("Done." dendl);
	}
	else // En cas d'échec de la connexion
	{
		cardOpened = false;
		Log.infonp ("Failed!" dendl);
		Log.error ("SD Initialisation failed! No SD logging..." dendl);

		return;
	}

	// Création du fichier log
	logFileAvailable = createLogFile();

	if (logFileAvailable) // Si la création du fichier n'a pas échouée
	{
		enabled = true;

		// Printing it error level so it is printed for any debug level
		// It's not a problem since it's not gonna print the prefix
		Log.errornp ("------------------------------------------------------------------------------------------------------------" endl);
		Log.errornp ("----------------------------------------------- SD log Start -----------------------------------------------" endl);
		Log.errornp ("------------------------------------------------------------------------------------------------------------" dendl);
	}
	else
	{
		enabled = false;
	}
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

boolean SdCard::cardIsOpened ()
{
	return cardOpened;
}

boolean SdCard::cardIsClosed ()
{
	return !cardOpened;
}

boolean SdCard::cardIsDetected ()
{
	return digitalRead (PIN_SD_CD);
}

boolean SdCard::cardIsUndetected ()
{
	return !digitalRead (PIN_SD_CD);
}

boolean SdCard::cardIsPresent ()
{
	return cardPresent;
}

boolean SdCard::cardIsAbsent ()
{
	return !cardPresent;
}

void SdCard::openFile ()
{
	if (!enabled)
		return;

	if (cardIsClosed())
	{
		if (!openCard()) // Ouverture de la connexion à la carte
			return;      // Si l'ouveture a échouée
	}

	if (fileIsClosed())
	{
		logFile = SD.open (sdFileName, FILE_WRITE);

		// If it failes, regenerate file name
		if (strlen (logFile.name()) < 1)
		{
			logFileAvailable = createLogFile();
		}

		if (!logFileAvailable) // If it fails again, just give up
		{
			enabled = false;
			return;
		}

		fileOpened = true;
		Log.verbose ("File opened" dendl);
	}

	fileLastOpened = millis();
}

boolean SdCard::openCard ()
{
	enabled = false;

	closeFile(); // Fermeture du fichier s'il est ouvert

	SD.end(); // Fermeture de la connexion à la carte SD
	cardOpened = false;

	// Ouverture d'une connexion à la carte SD
	if (SD.begin (PIN_SD_CS))
	{
		cardOpened = true;
	}
	else // En cas d'échec de la connexion
	{
		cardOpened = false;
		Log.error ("SD Initialisation failed! No SD logging..." dendl);

		return false;
	}

	// Création ou accès au fichier log
	enabled = logFileAvailable = createLogFile();

	return enabled;
} // SdCard::openCard

void SdCard::closeFile ()
{
	if (!enabled)
		return;

	if (fileIsOpened())
	{
		Log.verbose ("File closed" dendl);
		logFile.close();
		fileOpened = false;
	}
}

void SdCard::cardTests ()
{
	if (!SD_LOG_ENABLED)
		return;

	if (millis() - cardLastChecked >= CHECK_INTERVAL * 1000)
	{
		cardLastChecked = millis();

		if (cardIsPresent()) // If card was present
		{
			if (cardIsUndetected()) // But has been removed
			{
				enabled     = false;
				cardOpened  = false;
				cardPresent = false;

				Log.info ("SD Card removed, closing card... ");

				SD.end(); // Fermeture de la connexion à la carte SD

				Log.infonp ("Done." dendl);
			}
		}
		else // If card was absent
		{
			if (cardIsDetected()) // And a card is detected
			{
				cardPresent = true;

				init();
				fileLastOpened = millis(); // Prenvents from imediatly closing the card
			}
		}
	}

	if (enabled && cardIsOpened())
	{
		if (millis() - fileLastOpened >= FILE_CLOSE_TIME * 1000)
		{
			closeFile();
		}

		if (millis() - fileLastOpened >= CARD_CLOSE_TIME * 1000)
		{
			closeFile(); // In case it's not closed (Even though it should be)

			enabled = false; // Disabling SD so it's not printing on the card and looping closing the file
			Log.verbose ("Card closed" dendl);
			enabled = true; // And re-enable it after

			SD.end();
			cardOpened = false;
		}
	}
} // SdCard::cardTests

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
		Log.error ("%s log file failed! No logging..." dendl, loadingCreating);

		return fileOpened = false;
	}

	Log.infonp ("Done. (%s)" dendl, logFile.name());

	logFile.close();

	return fileOpened = true;
}

// A 13-char buffer is necessary
void SdCard::getLogFileName (char * buf)
{
	sprintf (buf, "%d%.2d%.2d.log", year(), month(), day());
}

SdCard sd = SdCard();
