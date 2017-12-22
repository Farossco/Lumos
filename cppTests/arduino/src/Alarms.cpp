#include "Alarms.h"
#include "ArduinoSerial.h"
#include "Logger.h"

Alarms::Alarms()
{
	for (int i = 0; i < N_PRAYER; i++)
		prayersSet[i] = 0;
}

void Alarms::initAll ()
{
	initTimeSync();
	initDawn();
	initPrayer();
}

void Alarms::initTimeSync ()
{
	clearTimeSync();

	timeSyncTimer = Alarm.timerRepeat (0, 60, 0, ArduinoSerial::askForTime);
}

void Alarms::initDawn ()
{
	if (!MORNING_ALARM_ENABLED || timeStatus() == timeNotSet)
		return;

	Log.trace ("Setting morning alarm" dendl);

	clearDawn();

	// Start the alarm a little time before
	morningAlarm = Alarm.alarmRepeat (WAKEUP_HOURS * 3600 + WAKEUP_MINUTES * 60 + WAKEUP_SECONDS - global.speed[MODE_DAWN] * 60, this->dawnStart);
}

void Alarms::initPrayer ()
{
	if (!PRAYER_ALARM_ENABLED || timeStatus() == timeNotSet)
		return;

	Log.trace ("Setting prayer alarms" dendl);

	clearPrayer();

	for (int i = 0; i < N_PRAYER; i++)
		prayerStartAlarm[i] = Alarm.alarmRepeat (prayerTime[i][0], prayerTime[i][1], 0, prayerStart);
}

void Alarms::clearAll ()
{
	clearTimeSync();
	clearDawn();
	clearPrayer();
}

void Alarms::clearTimeSync ()
{
	Alarm.free (timeSyncTimer);
}

void Alarms::clearDawn ()
{
	Alarm.free (morningAlarm);
}

void Alarms::clearPrayer ()
{
	for (int i = 0; i < N_PRAYER; i++)
		Alarm.free (prayerStartAlarm[i]);
}

void Alarms::prayerStart ()
{
	global.mode             = MODE_FADE;
	global.speed[MODE_FADE] = PRAYER_FADE_SPEED;
	global.on               = true;

	Log.info ("Starting prayer alert" endl);
	Log.trace ("It will stop in %d minutes..." dendl, PRAYER_ALERT_DURATION);

	// Stop alert in {PRAYER_ALERT_DURATION} minutes
	alarms.prayerStopAlarm = Alarm.timerOnce (PRAYER_ALERT_DURATION * 60, prayerStop);
}

void Alarms::prayerStop ()
{
	global.mode = MODE_DEFAULT;
	global.on   = false;

	Log.info ("Stopped prayer alert" dendl);
}

void Alarms::dawnStart ()
{
	global.mode = MODE_DAWN;
	global.on   = true;

	Log.info ("Starting dawn alert" dendl);
}

int Alarms::prayersAreSet ()
{
	int set = N_PRAYER;

	for (int i = 0; i < N_PRAYER; i++)
		if (prayersSet[i] == true)
			set--;

	return set;
}

Alarms alarms = Alarms();
