#include "arduino.h"

void initAlarmsAndTimers ()
{
	initTimeSyncTimer();
	initMorningAlarm();
	initPrayerAlarms();
}

void initTimeSyncTimer ()
{
	clearTimeSyncTimer();

	timeSyncTimer = Alarm.timerRepeat (0, 60, 0, askForTime);
}

void initMorningAlarm ()
{
	println (LEVEL_DEBUG, false);
	println (LEVEL_DEBUG, "Setting morning alarm");

	clearMorningAlarm();

	// Start the alarm a little time before
	morningAlarm = Alarm.alarmRepeat (WAKEUP_HOURS * 3600 + WAKEUP_MINUTES * 60 + WAKEUP_SECONDS - (speed[MODE_WAKEUP] / 4) * 60, morningStartAlarm);
}

void initPrayerAlarms ()
{
	println (false);
	println (LEVEL_DEBUG, "Setting prayer alarms");

	clearPrayerAlarms();

	for (int i = 0; i < N_PRAYER; i++)
		prayerStartAlarm[i] = Alarm.alarmRepeat (prayerTime[i][0], prayerTime[i][1], 0, prayerStart);
}

void clearAlarmsAndTimers ()
{
	clearTimeSyncTimer();
	clearMorningAlarm();
	clearPrayerAlarms();
}

void clearTimeSyncTimer ()
{
	Alarm.free (timeSyncTimer);
}

void clearMorningAlarm ()
{
	Alarm.free (morningAlarm);
}

void clearPrayerAlarms ()
{
	for (int i = 0; i < N_PRAYER; i++)
		Alarm.free (prayerStartAlarm[i]);
}

void prayerStart ()
{
	mode             = MODE_FADE;
	speed[MODE_FADE] = PRAYER_FADE_SPEED;
	on               = true;

	println (LEVEL_INFO, false);
	println (LEVEL_INFO, "Starting prayer alert");
	print (LEVEL_DEBUG, "It will stop in ");
	print (LEVEL_DEBUG, PRAYER_ALERT_DURATION, DEC, false);
	println (LEVEL_DEBUG, " minutes...", false);

	// Stop alert in {PRAYER_ALERT_DURATION} minutes
	prayerStopAlarm = Alarm.timerOnce (PRAYER_ALERT_DURATION * 60, prayerStop);
}

void prayerStop ()
{
	mode = MODE_DEFAULT;
	on   = false;

	println (LEVEL_INFO, false);
	println (LEVEL_INFO, "Stopped prayer alert");
}

void morningStartAlarm ()
{
	mode = MODE_WAKEUP;
	on   = true;

	println (LEVEL_INFO, false);
	println (LEVEL_INFO, "Starting wake up alert");
}
