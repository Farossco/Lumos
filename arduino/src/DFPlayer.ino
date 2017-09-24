#include "arduino.h"

void initDFPlayer ()
{
	if (!SOUND_ENABLED)
		return;

	println(LEVEL_DEBUG, false);
	println (LEVEL_DEBUG, "Initializing DFPlayer...");

	if (myDFPlayer.begin (Serial2)) // Use Serial2 to communicate with mp3.
	{
		println(LEVEL_INFO, false);
		println (LEVEL_INFO, "Done");
	}
	else
	{
		println(LEVEL_ERROR, false);
		println (LEVEL_ERROR, "DFPlayer init failed!");
		return;
	}

	myDFPlayer.volume (DEFAULT_VOLUME); // Set volume value. From 0 to 30
}
