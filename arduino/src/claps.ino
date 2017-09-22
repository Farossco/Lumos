#include "arduino.h"

// Lighting on double claps
void readClaps ()
{
	if (!CLAP_ENABLED)
		return;

	if (clapState == 0)
	{
		if (digitalRead (PIN_SOUND) == HIGH && millis() - endStateTime >= TIME_AFTER_START_OVER) // Start of first clap
		{
			clapState    = 1;
			endStateTime = millis();
			printlnNoPrefix(LEVEL_DEBUG);
			println (LEVEL_DEBUG, "Enter state 1 (Now waiting for the clap to end)");
		}
	}
	else if (clapState == 1)
	{
		if (millis() - endStateTime > MAX_CLAP_DURATION) // If clap duration reach maximum
		{
			clapState = 0;
			printlnNoPrefix(LEVEL_DEBUG);
			println (LEVEL_DEBUG, "Going back to state 0 (First clap lasted too long)");
		}

		if (digitalRead (PIN_SOUND) == LOW && millis() - endStateTime >= MIN_CLAP_DURATION) // End of first clap
		{
			clapState    = 2;
			endStateTime = millis();
			printlnNoPrefix(LEVEL_DEBUG);
			println (LEVEL_DEBUG, "Enter state 2 ( Now waiting a little bit...)");
		}
	}
	else if (clapState == 2)
	{
		if (digitalRead (PIN_SOUND) == HIGH) // Too soon second clap
		{
			clapState = 0;
			printlnNoPrefix(LEVEL_DEBUG);
			println (LEVEL_DEBUG, "Going back to state 0 ( Second clap started too soon)");
		}

		if (millis() - endStateTime >= MIN_TIME_BEETWIN_TWO_CLAPS)
		{
			clapState    = 3;
			endStateTime = millis();
			printlnNoPrefix(LEVEL_DEBUG);
			println (LEVEL_DEBUG, "Enter state 3 ( Now waiting for second clap to start)");
		}
	}
	else if (clapState == 3)
	{
		if (millis() - endStateTime >= MAX_TIME_BEETWIN_TWO_CLAPS + MIN_TIME_BEETWIN_TWO_CLAPS)
		{
			clapState = 0;
			printlnNoPrefix(LEVEL_DEBUG);
			println (LEVEL_DEBUG, "Going back to state 0 ( Waited too long for second clap)");
		}
		if (digitalRead (PIN_SOUND) == HIGH) // Start of second clap
		{
			clapState    = 4;
			endStateTime = millis();
			printlnNoPrefix(LEVEL_DEBUG);
			println (LEVEL_DEBUG, "Enter state 4 ( Now waiting for second clap to end)");
		}
	}
	else if (clapState == 4)
	{
		if (millis() - endStateTime >= MAX_CLAP_DURATION)
		{
			clapState = 0;
			printlnNoPrefix(LEVEL_DEBUG);
			println (LEVEL_DEBUG, "Going back to state 0 (Second clap lasted too long)");
		}
		if (digitalRead (PIN_SOUND) == LOW && millis() - endStateTime >= MIN_CLAP_DURATION) // End of second clap
		{
			on           = !on;
			clapState    = 0;
			endStateTime = millis();
			printlnNoPrefix(LEVEL_INFO);
			println (LEVEL_INFO, "Double clapped, switching " + on ? "on" : "off");
		}
	}
} // readClaps
