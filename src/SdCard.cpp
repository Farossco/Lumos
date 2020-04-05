#if defined(LUMOS_ARDUINO_MEGA)

#include <stdlib.h>
#include <Time.h>
#include "SdCard.h"
#include "Logger.h"

SdCard::SdCard()
{
	pinMode (PIN_SD_LED_RED, OUTPUT);
	pinMode (PIN_SD_LED_GREEN, OUTPUT);
	pinMode (PIN_SD_LED_BLUE, OUTPUT);

	lightOff();

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
	enabled = false;

	tempDisabled = true;

	closeFile(); // Fermeture du fichier s'il est ouvert

	SD.end(); // Fermeture de la connexion à la carte SD
	cardOpened = false;

	if (!cardIsDetected())
	{
		cardPresent = false;
		warn << "SD card undetected, no SD logging..." << dendl;
		lightNoCard();
	}
	else
	{
		cardPresent = true;
		inf << "SD card detected, initializing... ";

		// Ouverture d'une connexion à la carte SD
		if (!SD.begin (PIN_SD_CS)) // En cas d'échec de la connexion
		{
			cardOpened = false;
			inf << "Failed!" << dendl;
			err << "SD Initialisation failed! No SD logging..." << dendl;
			lightError();
		}
		else
		{
			cardOpened = true;

			inf << "Done." << dendl;

			// Création du fichier log
			logFileAvailable = createLogFile();

			if (!logFileAvailable) // Si la création du fichier a échouée
				lightError();
			else
			{
				enabled = true;
				lightConnected();

				// Printing it error level so it is printed for any debug level
				// It's not a problem since it's not going to print the prefix
				err << np << "------------------------------------------------------------------------------------------------------------" << endl;
				err << np << "----------------------------------------------- SD log Start -----------------------------------------------" << endl;
				err << np << "------------------------------------------------------------------------------------------------------------" << dendl;
			}
		}
	}

	tempDisabled = false;
} // SdCard::init

File & SdCard::getFile ()
{
	return logFile;
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

	tempDisabled = true;

	if (cardIsClosed())
		openCard();  // Ouverture de la connexion à la carte

	if (cardIsOpened() && fileIsClosed())
	{
		logFileAvailable = createLogFile();

		if (!logFileAvailable) // If it fails again, just give up
		{
			enabled = false;
			lightError();
		}
		else
		{
			fileOpened = true;
			verb << "File opened" << dendl;
		}
	}

	fileLastOpened = millis();

	tempDisabled = false;
}

void SdCard::closeFile ()
{
	if (!enabled)
		return;

	if (fileIsOpened())
	{
		tempDisabled = true;
		verb << "File closed" << dendl;
		logFile.close();
		fileOpened   = false;
		tempDisabled = false;
	}
}

boolean SdCard::openCard ()
{
	tempDisabled = true;

	closeFile(); // Fermeture du fichier s'il est ouvert

	SD.end(); // Fermeture de la connexion à la carte SD
	cardOpened = false;

	// Ouverture d'une connexion à la carte SD
	if (SD.begin (PIN_SD_CS))
	{
		verb << "Card opened" << dendl;
		lightConnected();

		cardOpened = true;
		enabled    = true;
	}
	else // En cas d'échec de la connexion
	{
		cardOpened = false;
		err << "SD Initialisation failed! No SD logging..." << dendl;
		lightError();

		enabled = false;
	}

	tempDisabled = false;

	return enabled;
} // SdCard::openCard

void SdCard::cardTests ()
{
	tempDisabled = true;

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

				lightNoCard();

				inf << "SD Card removed, closing card... ";

				SD.end(); // Fermeture de la connexion à la carte SD

				inf << "Done." << dendl;
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
		if (millis() - fileLastOpened >= FILE_CLOSE_TIME * 1000) // Fermeture automatique du fichier
		{
			closeFile();
		}

		if (millis() - fileLastOpened >= CARD_CLOSE_TIME * 1000) // Fermeture automatique de la connexion à la carte SD
		{
			closeFile(); // In case it's not closed (Even though it should be)

			verb << "Card closed" << dendl;

			lightIdle();

			SD.end();
			cardOpened = false;
		}
	}

	tempDisabled = false;
} // SdCard::cardTests

boolean SdCard::isEnabled ()
{
	return enabled && !tempDisabled;
}

boolean SdCard::createLogFile ()
{
	tempDisabled = true;

	getLogFileName (sdFileName);

	const char * loadingCreating = SD.exists (sdFileName) ? "Loading" : "Creating";

	verb << loadingCreating << " log file... ";

	logFile = SD.open (sdFileName, FILE_WRITE);

	if (strlen (logFile.name()) < 1)
	{
		verb << "Failed!" << dendl;
		err << loadingCreating << " log file failed! No logging..." << dendl;

		fileOpened = false;
	}
	else
	{
		verb << "Done. (" << logFile.name() << ")" << dendl;
		logFile.close();
		fileOpened = true;
	}

	tempDisabled = false;

	return fileOpened;
}

// A 13-char buffer is necessary
void SdCard::getLogFileName (char * buf)
{
	sprintf (buf, "%d%.2d%.2d.log", year(), month(), day());
}

void SdCard::lightError ()
{
	analogWrite (PIN_SD_LED_RED, SD_LED_POWER);
	analogWrite (PIN_SD_LED_GREEN, LOW);
	analogWrite (PIN_SD_LED_BLUE, LOW);
}

void SdCard::lightConnected ()
{
	analogWrite (PIN_SD_LED_RED, LOW);
	analogWrite (PIN_SD_LED_GREEN, SD_LED_POWER);
	analogWrite (PIN_SD_LED_BLUE, LOW);
}

void SdCard::lightIdle ()
{
	analogWrite (PIN_SD_LED_RED, SD_LED_POWER / 2);
	analogWrite (PIN_SD_LED_GREEN, SD_LED_POWER / 2);
	analogWrite (PIN_SD_LED_BLUE, LOW);
}

void SdCard::lightNoCard ()
{
	analogWrite (PIN_SD_LED_RED, SD_LED_POWER / 2);
	analogWrite (PIN_SD_LED_GREEN, LOW);
	analogWrite (PIN_SD_LED_BLUE, SD_LED_POWER / 2);
}

void SdCard::lightOff ()
{
	digitalWrite (PIN_SD_LED_RED, LOW);
	digitalWrite (PIN_SD_LED_GREEN, LOW);
	digitalWrite (PIN_SD_LED_BLUE, LOW);
}

SdCard sd = SdCard();

#endif // if defined(LUMOS_ARDUINO_MEGA)