// ******* readClaps ******* //
int clapState;              // Same as "state" but for claps
unsigned long endStateTime; // Time position at the end of a state (Allow time counting)

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
			println ("Enter state 1 (Now waiting for the clap to end)\n");
		}
	}
	else if (clapState == 1)
	{
		if (millis() - endStateTime > MAX_CLAP_DURATION) // If clap duration reach maximum
		{
			clapState = 0;
			println ("Going back to state 0 (First clap lasted too long)\n");
		}

		if (digitalRead (PIN_SOUND) == LOW && millis() - endStateTime >= MIN_CLAP_DURATION) // End of first clap
		{
			clapState    = 2;
			endStateTime = millis();
			println ("Enter state 2 ( Now waiting a little bit...)\n");
		}
	}
	else if (clapState == 2)
	{
		if (digitalRead (PIN_SOUND) == HIGH) // Too soon second clap
		{
			clapState = 0;
			println ("Going back to state 0 ( Second clap started too soon)\n");
		}

		if (millis() - endStateTime >= MIN_TIME_BEETWIN_TWO_CLAPS)
		{
			clapState    = 3;
			endStateTime = millis();
			println ("Enter state 3 ( Now waiting for second clap to start)\n");
		}
	}
	else if (clapState == 3)
	{
		if (millis() - endStateTime >= MAX_TIME_BEETWIN_TWO_CLAPS + MIN_TIME_BEETWIN_TWO_CLAPS)
		{
			clapState = 0;
			println ("Going back to state 0 ( Waited too long for second clap)\n");
		}
		if (digitalRead (PIN_SOUND) == HIGH) // Start of second clap
		{
			clapState    = 4;
			endStateTime = millis();
			println ("Enter state 4 ( Now waiting for second clap to end)\n");
		}
	}
	else if (clapState == 4)
	{
		if (millis() - endStateTime >= MAX_CLAP_DURATION)
		{
			clapState = 0;
			println ("Going back to state 0 (Second clap lasted too long)\n");
		}
		if (digitalRead (PIN_SOUND) == LOW && millis() - endStateTime >= MIN_CLAP_DURATION) // End of second clap
		{
			on           = !on;
			clapState    = 0;
			endStateTime = millis();
			println ("Switching LED !\n");
		}
	}
} // readClaps
