#ifndef SDCARD_H
#define SDCARD_H

#include <SD.h>

const float FILE_CLOSE_TIME = 1;
const float CARD_CLOSE_TIME = 10;
const float CHECK_INTERVAL  = 0;

const int PIN_SD_CS        = 49; // Chip Select
const int PIN_SD_CD        = 53; // Card Detect
const int PIN_SD_LED_RED   = 44; // RGB Red pin
const int PIN_SD_LED_GREEN = 45; // RGB Green pin
const int PIN_SD_LED_BLUE  = 46; // RGB Blue pin
const int SD_INDICATOR_POWER  = 10;

class SdCard
{
public:
	SdCard();
	void init ();
	File * getFile ();
	boolean isEnabled ();
	boolean fileIsOpened ();
	boolean fileIsClosed ();
	boolean cardIsOpened ();
	boolean cardIsClosed ();
	boolean cardIsDetected ();
	boolean cardIsUndetected ();
	boolean cardIsPresent ();
	boolean cardIsAbsent ();
	void openFile ();
	boolean openCard ();
	void closeFile ();
	void cardTests ();
	void lightError ();
	void lightConnected ();
	void lightIdle ();
	void lightNoCard ();
	void lightOff ();

private:
	File logFile;
	boolean logFileAvailable, fileOpened, cardOpened, cardPresent;
	char sdFileName[13];
	unsigned long fileLastOpened, cardLastChecked;
	boolean enabled, tempDisabled;

	boolean createLogFile ();
	void getLogFileName (char * buf);
};

extern SdCard sd;

#endif // ifndef SDCARD_H
