#ifndef ALARMS_H
#define ALARMS_H

#include "Global.h"
#include <TimeAlarms.h>

class Alarms
{
public:
	Alarms();

	void initAll ();
	void initTimeSync ();
	void initDawn ();
	void initPrayer ();

	void clearAll ();
	void clearTimeSync ();
	void clearDawn ();
	void clearPrayer ();

	int prayersAreSet ();

	int prayerTime[N_PRAYER][3]; // [0] = Hours / [1] = Minutes / [2] = Hours * 60 + Minutes
	boolean prayersSet[N_PRAYER];

	AlarmId morningAlarm;
	AlarmId prayerStartAlarm[N_PRAYER];
	AlarmId prayerStopAlarm;
	AlarmId timeSyncTimer;

private:
	static void prayerStart ();
	static void prayerStop ();
	static void dawnStart ();
};

extern Alarms alarms;

#endif // ifndef ALARMS_H
