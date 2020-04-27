#if defined(LUMOS_ARDUINO_MEGA)

#include <Time.h>
#include "SdCard.h"
#include "ArduinoLogger.h"

SdCard::SdCard()
{
	pinMode (PIN_SD_LED_RED, OUTPUT);
	pinMode (PIN_SD_LED_GREEN, OUTPUT);
	pinMode (PIN_SD_LED_BLUE, OUTPUT);
	pinMode (PIN_SD_CS, INPUT);
	pinMode (PIN_SD_CD, INPUT_PULLUP);

	lightOff();

	enabled     = false;
	cardPresent = false;
	flushTimer  = 0;
}

void SdCard::init ()
{
	if (detectCard())
	{
		cardPresent = true;

		openCard();
	}
	else
	{
		cardPresent = false;
		warn << "SD card undetected, no SD logging" << dendl;
		lightNoCard();
	}
}

void SdCard::openCard ()
{
	inf << "SD card detected" << endl;

	delay (2000); // Waiting to make sure the card is properly connected

	inf << "Initializing... ";

	if (SD.begin (PIN_SD_CS))
	{
		inf << "Done." << dendl;

		if (createLogFile())
		{
			lightConnected();

			logger.add (file, LOG_LEVEL_VERBOSE);

			// Printing it with error level so it is printed for any debug level
			// It's not a problem since it's not going to print the prefix

			err << dsb (Serial) << np << "------------------------------------------------------------------------------------------------------------" << endl;
			err << dsb (Serial) << np << "----------------------------------------------- SD log Start -----------------------------------------------" << endl;
			err << dsb (Serial) << np << "------------------------------------------------------------------------------------------------------------" << dendl;

			enabled = true;
		}
		else
		{
			lightError();
		}
	}
	else
	{
		inf << "Failed!" << dendl;
		err << "SD card opening failed! No SD logging" << dendl;
		lightError();
	}
} // SdCard::openCard

boolean SdCard::detectCard ()
{
	return digitalRead (PIN_SD_CD);
}

void SdCard::action ()
{
	autoDetect();

	if (enabled)
		autoFlush();
}

void SdCard::autoDetect ()
{
	if (cardPresent) // If card was present
	{
		if (!detectCard()) // But has been removed
		{
			enabled     = false;
			cardPresent = false;

			lightNoCard();

			inf << "SD Card removed, closing card... ";

			logger.disable (file);

			SD.end(); // Fermeture de la connexion à la carte SD

			inf << "Done." << dendl;
		}
	}
	else // If card was absent
	{
		if (detectCard()) // And a card is detected
		{
			cardPresent = true;

			openCard();

			flushTimer = millis(); // Prenvents from imediatly flushing the card
		}
	}
}

void SdCard::autoFlush ()
{
	if (millis() - flushTimer >= FILE_FLUSH_TIME * 1000)
	{
		file.flush();

		flushTimer = millis();
	}
}

boolean SdCard::createLogFile ()
{
	char name[13];
	const char * loadingCreating;

	sprintf (name, "%d%.2d%.2d.log", year(), month(), day());

	loadingCreating = SD.exists (name) ? "Loading" : "Creating";

	trace << loadingCreating << " log file... ";

	file = SD.open (name, FILE_WRITE);

	if (!file)
	{
		verb << "Failed!" << dendl;
		err << loadingCreating << " log file failed! No logging..." << dendl;

		return false;
	}

	verb << "Done. (" << file.name() << ")" << dendl;

	return true;
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