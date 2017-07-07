// ******* Prayer ******* //
int flagEnter; // If we have to start fading
int flagLeave; // If we have to stop fading
int faded;     // If we already gave the order to start fading
int unfaded;   // If we already gave the order to stop fading

// ******* Wake up ******* //
int wokeUp;    // If we already gave the order to wake up
int timeAsked; // If we already asked for time

// ******* Global ******* //
int i, j; // Just counting variables

void onPrayerTime ()
{
	flagEnter = false;
	flagLeave = false;

	for (i = 0; i < N_PRAYER; i++)
		if ((hour() * 60 + minute()) == prayerTime[i][2])
		{
			flagEnter = true;
			break;
		}

	for (j = 0; j < N_PRAYER; j++)
		if ((hour() * 60 + minute()) == (prayerTime[j][2] + 10))
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
	mode = MODE_FADE;
	speed[MODE_FADE] = 97;
	on    = true;
	faded = true;

	print ("Time to pray ");
	printNoPrefix (prayersName[i]);
	printlnNoPrefix (" !");
	print ("It will stop at ");
	printNoPrefix ((prayerTime[i][2] + 10) / 60, DEC);
	printNoPrefix (":");
	printNoPrefix ((prayerTime[i][1] + 10) % 60, DEC);
	printNoPrefix (" (");
	printNoPrefix (prayerTime[i][2] + 10, DEC);
	printNoPrefix (")\n");
}

void prayerStop ()
{
	mode    = MODE_DEFAULT;
	on      = false;
	unfaded = true;

	print ("Stop ");
	printNoPrefix (prayersName[j]);
	printlnNoPrefix (" alert\n");
}

// Test wakeup time and peak hours for resynchronization
void testWakeUpTime ()
{
	// If actual time coorespond with wakeup time
	if (hour() == prayerTime[0][0] && minute() == prayerTime[0][1] && !wokeUp)
	{
		mode   = MODE_WAKEUP;
		on     = true;
		wokeUp = true;

		println ("Wake up !\n");
	}
	else if (hour() != prayerTime[0][0] || minute() != prayerTime[0][1])
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
