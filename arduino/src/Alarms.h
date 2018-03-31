#ifndef ALARMS_H
#define ALARMS_H

#include <TimeAlarms.h>

const boolean MORNING_ALARM_ENABLED = true; // Enable morning alarm

// Wake up
const int WAKEUP_HOURS   = 06;
const int WAKEUP_MINUTES = 30;
const int WAKEUP_SECONDS = 00;

class Alarms
{
public:
	Alarms();

	void initAll ();
	void initTimeSync ();
	void initDawn ();

	void clearAll ();
	void clearTimeSync ();
	void clearDawn ();

	AlarmId morningAlarm;
	AlarmId timeSyncTimer;

private:
	static void dawnStart ();
};

extern Alarms alarms;

#endif // ifndef ALARMS_H
