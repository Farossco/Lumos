#include "arduino.h"

void initDFPlayer ()
{
	if (!SOUND_ENABLED)
		return;

	println (LEVEL_INFO, false);
	print (LEVEL_INFO, "Initializing DFPlayer... ");

	if (myDFPlayer.begin (Serial2)) // Use Serial2 to communicate with mp3.
	{
		println (LEVEL_INFO, "Done.");
	}
	else
	{
		println (LEVEL_INFO, "Failed!", false);
		println (LEVEL_ERROR, false);
		println (LEVEL_ERROR, "DFPlayer init failed! No sound for this session...");
		return;
	}

	println (LEVEL_DEBUG, false);
	print (LEVEL_DEBUG, "Setting volume to ");
	print (LEVEL_DEBUG, DEFAULT_VOLUME, false);
	print (LEVEL_DEBUG, "/", false);
	println (LEVEL_DEBUG, MAX_VOLUME, false);
	myDFPlayer.volume (DEFAULT_VOLUME); // Set volume value. From MIN_VOLUME to MAX_VOLUME
}
