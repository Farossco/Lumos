#include "SdCard.h"
#include "ArduinoLogger.h"

SdCard::SdCard()
{
	pinMode(PIN_SD_LED_RED,   OUTPUT);
	pinMode(PIN_SD_LED_GREEN, OUTPUT);
	pinMode(PIN_SD_LED_BLUE,  OUTPUT);
	pinMode(PIN_SD_CS, INPUT);
	pinMode(PIN_SD_CD, INPUT_PULLUP);

	lightOff();

	cardOpened   = false;
	cardDetected = false;
	attempts     = 0;
}

void SdCard::init()
{
	if (detectCard()) {
		openCard();
	} else {
		warn << "SD card undetected, no SD logging" << dendl;
		lightNoCard();
	}
}

void SdCard::openCard()
{
	attempts++;

	inf << "Opening card (Attempt n°" << attempts << ")... ";

	if (SD.begin(PIN_SD_CS)) { /* Card successfully opened */
		attempts = 0;

		inf << "Done." << dendl;

		if (createLogFile()) {
			lightConnected();

			logger.add(file, LOG_LEVEL_VERBOSE);

			/* Printing it with error level so it is printed for any debug level */
			/* It's not a problem since it's not going to print the prefix */
			err << dsb(Serial) << np << "------------------------------------------------------------------------------------------------------------" << endl;
			err << dsb(Serial) << np << "----------------------------------------------- SD log Start -----------------------------------------------" << endl;
			err << dsb(Serial) << np << "------------------------------------------------------------------------------------------------------------" << dendl;

			cardOpened = true;
		} else {
			lightError();
		}
	} else { /* Card failed to opened */
		lightError();

		inf << "Failed";

		if (attempts < SD_ATTEMPTS && detectCard()) {
			inf << ", reattempting..." << endl;
			openCard();
		} else {
			inf << endl;
			err << "SD card opening failed! No SD logging" << dendl;
			attempts = 0;
		}
	}
} /* SdCard::openCard */

void SdCard::closeCard()
{
	lightNoCard();

	logger.disable(file);

	inf << "Closing card... ";

	SD.end();

	cardOpened = false;

	inf << "Done." << dendl;
}

boolean SdCard::detectCard()
{
	return cardDetected = digitalRead(PIN_SD_CD);
}

void SdCard::action()
{
	autoDetect();

	if (cardOpened)
		file.flush();
}

void SdCard::autoDetect()
{
	if (cardDetected) {      /* If card was present */
		if (!detectCard()) { /* But has been removed */
			inf << dsb(file) << "SD card removed" << endl;

			closeCard();
		}
	} else {                /* If card was absent */
		if (detectCard()) { /* And a card is detected */
			lightIdle();

			inf << "SD card detected" << endl;

			delay(1000); /* Waiting to make sure the card is properly connected */

			openCard();
		}
	}
}

boolean SdCard::createLogFile()
{
	char name[13];
	const char *loadingCreating;

	sprintf(name, "%d%.2d%.2d.log", year(), month(), day());

	loadingCreating = SD.exists(name) ? "Loading" : "Creating";

	trace << loadingCreating << " log file... ";

	file = SD.open(name, FILE_WRITE);

	if (!file) {
		trace << "Failed!" << dendl;
		err << loadingCreating << " log file failed! No logging..." << dendl;

		return false;
	}

	trace << "Done. (" << file.name() << ")" << dendl;

	return true;
}

void SdCard::lightError()
{
	/*analogWrite (PIN_SD_LED_RED, SD_LED_POWER);
	 * analogWrite (PIN_SD_LED_GREEN, LOW);
	 * analogWrite (PIN_SD_LED_BLUE, LOW);*/
}

void SdCard::lightConnected()
{
	/*analogWrite (PIN_SD_LED_RED, LOW);
	 * analogWrite (PIN_SD_LED_GREEN, SD_LED_POWER);
	 * analogWrite (PIN_SD_LED_BLUE, LOW);*/
}

void SdCard::lightIdle()
{
	/*analogWrite (PIN_SD_LED_RED, LOW);
	 * analogWrite (PIN_SD_LED_GREEN, LOW);
	 * analogWrite (PIN_SD_LED_BLUE, SD_LED_POWER);*/
}

void SdCard::lightNoCard()
{
	/*analogWrite (PIN_SD_LED_RED, SD_LED_POWER / 2);
	 * analogWrite (PIN_SD_LED_GREEN, LOW);
	 * analogWrite (PIN_SD_LED_BLUE, SD_LED_POWER / 2);*/
}

void SdCard::lightOff()
{
	digitalWrite(PIN_SD_LED_RED,   LOW);
	digitalWrite(PIN_SD_LED_GREEN, LOW);
	digitalWrite(PIN_SD_LED_BLUE,  LOW);
}

SdCard sd = SdCard();
