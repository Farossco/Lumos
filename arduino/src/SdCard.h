#ifndef SDCARD_H
#define SDCARD_H

#include <SD.h>

const float FILE_CLOSE_TIME = 1;
const float CARD_CLOSE_TIME = 10;
const float CHECK_INTERVAL  = 5;

const int PIN_SD_CS = 49; // Chip Select
const int PIN_SD_CD = 53; // Card Detect

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

private:
	File logFile;
	boolean logFileAvailable, fileOpened, cardOpened, cardPresent;
	char sdFileName[13];
	unsigned long fileLastOpened, cardLastChecked;
	boolean enabled;

	boolean createLogFile ();
	void getLogFileName (char * buf);
};

extern SdCard sd;

#endif // ifndef SDCARD_H
