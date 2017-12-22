#ifndef SDCARD_H
#define SDCARD_H

#include <SD.h>

class SdCard
{
public:
	SdCard();
	void init (int pin);

private:
	int pin;
	File logFile;
	boolean logFileAvailable;
	char sdFileName[13];

	boolean createLogFile ();
	void getLogFileName (char * buf);
};

extern SdCard sd;

#endif // ifndef SDCARD_H
