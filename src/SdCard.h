#ifndef SD_CARD_H
#define SD_CARD_H

#include <SD.h>

const uint8_t PIN_SD_CS        = 10; /* Chip Select */
const uint8_t PIN_SD_CD        = 9;  /* Card Detect */
const uint8_t PIN_SD_LED_RED   = 3;  /* RGB Red pin */
const uint8_t PIN_SD_LED_GREEN = 4;  /* RGB Green pin */
const uint8_t PIN_SD_LED_BLUE  = 5;  /* RGB Blue pin */
const uint8_t SD_LED_POWER     = 255;
const uint8_t SD_ATTEMPTS      = 3; /* The number of attempts at opening the card */

class SdCard {
public:
	SdCard();
	void init();
	void action();
	File file;

private:
	boolean cardDetected;
	bool cardOpened;
	uint8_t attempts;

	void openCard();
	void closeCard();
	bool detectCard();
	void lightError();
	void lightConnected();
	void lightIdle();
	void lightNoCard();
	void lightOff();
	void autoDetect();

	bool createLogFile();
};

extern SdCard sd;

#endif // ifndef SDCARD_H
