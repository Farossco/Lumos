#include "arduino.h"

void initDFPlayer ()
{
	if (!SOUND_ENABLED)
		return;

	printlnNoPrefix(LEVEL_DEBUG);
	println (LEVEL_DEBUG, "Initializing DFPlayer...");

	if (myDFPlayer.begin (Serial2)) // Use Serial2 to communicate with mp3.
	{
		printlnNoPrefix(LEVEL_INFO);
		println (LEVEL_INFO, "Done");
	}
	else
	{
		printlnNoPrefix(LEVEL_ERROR);
		println (LEVEL_ERROR, "DFPlayer init failed!");
		return;
	}

	myDFPlayer.volume (DEFAULT_VOLUME); // Set volume value. From 0 to 30
}
