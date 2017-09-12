#include "arduino.h"

void initSdCard ()
{
	printlnNoPrefix();
	print ("Initializing SD card... ");

	if (!SD.begin (PIN_SD_CS))
	{
		printlnNoPrefix ("Failed! No logging for this session...");
		return;
	}
	printlnNoPrefix ("Done.");

	logFileAvailable = createLogFile();
}

boolean createLogFile ()
{
	print ("Creating log file... ");

	logFile = SD.open (getLogFileName(), FILE_WRITE);

	if (strlen (logFile.name()) < 1)
	{
		printlnNoPrefix ("Failed! No logging for this session...");
		return false;
	}

	printNoPrefix ("Done. (");
	printNoPrefix (logFile.name());
	printlnNoPrefix (")");

	logFile.close();

	return true;
}

char * getLogFileName ()
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

	name.toCharArray (sdFileName, 13);

	return sdFileName;
}
