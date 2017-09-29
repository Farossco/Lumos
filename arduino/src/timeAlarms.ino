#include "arduino.h"

void initTimeAlarms ()
{
	clearAlarms();

	timeSyncTimer = Alarm.timerRepeat (0, 60, 0, askForTime);

	wakeUpAlarm = Alarm.alarmRepeat (WAKEUP_HOURS, WAKEUP_MINUTES, WAKEUP_SECONDS, morningAlarm);

	for (int i = 0; i < N_PRAYER; i++)
	{
		prayerStartAlarm[i] = Alarm.alarmRepeat (prayerTime[i][0], prayerTime[i][1], 0, prayerStart);
		prayerStopAlarm[i]  = Alarm.alarmRepeat (prayerTime[i][0], prayerTime[i][1] + 10, 0, prayerStop);
	}
}

void clearAlarms ()
{
	boolean cleared = false;

	if (Alarm.isAllocated (timeSyncTimer))
	{
		Alarm.free (timeSyncTimer);
		cleared = true;
	}

	if (Alarm.isAllocated (wakeUpAlarm))
	{
		Alarm.free (wakeUpAlarm);
		cleared = true;
	}

	for (int i = 0; i < N_PRAYER; i++)
	{
		if (Alarm.isAllocated (prayerStartAlarm[i]))
		{
			Alarm.free (prayerStartAlarm[i]);
			cleared = true;
		}

		if (Alarm.isAllocated (prayerStopAlarm[i]))
		{
			Alarm.free (prayerStopAlarm[i]);
			cleared = true;
		}
	}

	if (cleared)
	{
		println (LEVEL_DEBUG, false);
		println (LEVEL_DEBUG, "Alarms cleared");
	}
	else
	{
		println (LEVEL_DEBUG, false);
		println (LEVEL_DEBUG, "Nothing to clear");
	}
} // clearAlarms

void prayerStart ()
{
	mode             = MODE_FADE;
	speed[MODE_FADE] = PRAYER_FADE_SPEED;
	on               = true;

	println (LEVEL_INFO, false);
	println (LEVEL_INFO, "Starting prayer alert");
}

void prayerStop ()
{
	mode = MODE_DEFAULT;
	on   = false;

	println (LEVEL_INFO, false);
	println (LEVEL_INFO, "Stopped prayer alert");
}

// Test wakeup time and peak hours for resynchronization
void morningAlarm ()
{
	mode = MODE_WAKEUP;
	on   = true;

	println (LEVEL_INFO, false);
	println (LEVEL_INFO, "Starting wake up alert");
}
