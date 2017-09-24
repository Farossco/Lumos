#include "arduino.h"

void testPrayerTime ()
{
	flagEnter = false;
	flagLeave = false;

	for (prayerIndexStart = 0; prayerIndexStart < N_PRAYER; prayerIndexStart++)
		if ((hour() * 60 + minute()) == prayerTime[prayerIndexStart][2])
		{
			flagEnter = true;
			break;
		}

	for (prayerIndexStop = 0; prayerIndexStop < N_PRAYER; prayerIndexStop++)
		if ((hour() * 60 + minute()) == (prayerTime[prayerIndexStop][2] + 10))
		{
			flagLeave = true;
			break;
		}


	if (flagEnter && !faded)
		prayerStart();
	else if (!flagEnter)
		faded = false;

	if (flagLeave && !unfaded)
		prayerStop();
	else if (!flagLeave)
		unfaded = false;
}

void prayerStart ()
{
	mode             = MODE_FADE;
	speed[MODE_FADE] = 97;
	on               = true;
	faded            = true;

	println(LEVEL_INFO, false);
	print (LEVEL_INFO, "Started");
	print (LEVEL_INFO, PRAYERS_NAME[prayerIndexStart], false);
	println (LEVEL_INFO, " alert", false);
	print (LEVEL_DEBUG, "It will stop at ");
	print (LEVEL_DEBUG, (prayerTime[prayerIndexStart][2] + 10) / 60, DEC, false);
	print (LEVEL_DEBUG, ":", false);
	print (LEVEL_DEBUG, (prayerTime[prayerIndexStart][1] + 10) % 60, DEC, false);
	print (LEVEL_DEBUG, " (", false);
	print (LEVEL_DEBUG, prayerTime[prayerIndexStart][2] + 10, DEC, false);
	println (LEVEL_DEBUG, ")", false);
}

void prayerStop ()
{
	mode    = MODE_DEFAULT;
	on      = false;
	unfaded = true;

	println(LEVEL_INFO, false);
	print (LEVEL_INFO, "Stopped ");
	print (LEVEL_INFO, PRAYERS_NAME[prayerIndexStop], false);
	println (LEVEL_INFO, " alert", false);
}

// Test wakeup time and peak hours for resynchronization
void testWakeUpTime ()
{
	// If actual time coorespond with wakeup time
	if (hour() == WAKEUP_HOURS && minute() == WAKEUP_MINUTES && !wokeUp)
	{
		mode   = MODE_WAKEUP;
		on     = true;
		wokeUp = true;

		println (LEVEL_INFO, false);
		println (LEVEL_INFO, "Starting wake up alert");
	}
	else if ((hour() != WAKEUP_HOURS || minute() != WAKEUP_MINUTES) && wokeUp)
	{
		wokeUp = false;
	}
}

void peakTime ()
{
	// If we are on a peak time, we ask for time to make sure it's right
	if (minute() == 0 && !timeAsked)
	{
		askForTime();
		timeAsked = true;
	}
	else if (minute() != 0)
	{
		timeAsked = false;
	}
}
