#include "arduino.h"

void initSdCard()
{
	printlnNoPrefix(LEVEL_INFO);
	print(LEVEL_INFO, "Initializing SD card... ");

	if (SD.begin(PIN_SD_CS))
	{
		printlnNoPrefix(LEVEL_INFO, "Done.");
	}
	else
	{
		printlnNoPrefix(LEVEL_INFO);
		printlnNoPrefix(LEVEL_ERROR);
		println(LEVEL_ERROR, "SD Initialisation failed! No logging for this session...");
		return;
	}

	logFileAvailable = createLogFile();
}

boolean createLogFile()
{
	String loadingCreating = SD.exists(getLogFileName()) ? "Loading" : "Creating";

	printlnNoPrefix(LEVEL_INFO);

	print(LEVEL_INFO, loadingCreating + " log file... ");

	logFile = SD.open(getLogFileName(), FILE_WRITE);

	if (strlen(logFile.name()) < 1)
	{
		printlnNoPrefix(LEVEL_ERROR);
		println(LEVEL_ERROR, loadingCreating + " log file failed! No logging for this session...");
		return false;
	}

	printNoPrefix(LEVEL_INFO, "Done.");
	printlnNoPrefix(LEVEL_DEBUG, " (");
	printNoPrefix(LEVEL_DEBUG, logFile.name());
	printlnNoPrefix(LEVEL_DEBUG, ")");

	logFile.close();

	return true;
}

char *getLogFileName()
{
	String name = "";

	name += year();
	if (month() < 10)
		name += 0;
	name += month();
	if (day() < 10)
		name += 0;
	name += day();
	name += ".log";

	name.toCharArray(sdFileName, 13);

	return sdFileName;
}
