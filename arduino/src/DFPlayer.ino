#include "arduino.h"

void initDFPlayer ()
{
	if (!SOUND_ENABLED)
		return;

	println ("Initializing DFPlayer...");

	if (!myDFPlayer.begin (Serial2)) // Use Serial2 to communicate with mp3.
	{
		println ("Unable to begin");
	}

	println (F ("DFPlayer Mini online."));

	myDFPlayer.volume (DEFAULT_VOLUME); // Set volume value. From 0 to 30

	printlnNoPrefix();
}
