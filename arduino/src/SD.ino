#include "arduino.h"

void initSdCard()
{
	println(LEVEL_INFO, false);
	print(LEVEL_INFO, "Initializing SD card... ");

	if (SD.begin(PIN_SD_CS))
	{
		println(LEVEL_INFO, "Done.", false);
	}
	else
	{
		println(LEVEL_INFO, false);
		println(LEVEL_ERROR, false);
		println(LEVEL_ERROR, "SD Initialisation failed! No logging for this session...");
		return;
	}

	logFileAvailable = createLogFile();
}

boolean createLogFile()
{
	String loadingCreating = SD.exists(getLogFileName()) ? "Loading" : "Creating";

	println(LEVEL_INFO, false);

	print(LEVEL_INFO, loadingCreating + " log file... ");

	logFile = SD.open(getLogFileName(), FILE_WRITE);

	if (strlen(logFile.name()) < 1)
	{
		println(LEVEL_ERROR, false);
		println(LEVEL_ERROR, loadingCreating + " log file failed! No logging for this session...");
		return false;
	}

	print(LEVEL_INFO, "Done.", false);
	print(LEVEL_DEBUG, " (", false);
	print(LEVEL_DEBUG, logFile.name(), false);
	print(LEVEL_DEBUG, ")", false);
	println(LEVEL_INFO, false);

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
