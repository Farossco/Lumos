#ifndef ALARMS_H
#define ALARMS_H

#include <TimeLib.h>

const bool MORNING_ALARM_ENABLED = true; // Enable morning alarm

// Wake up
const int DEFAULT_DAWN_TIME = 8 * 60 + 30; // 08h30

class Alarms
{
public:
	Alarms();

	void init ();
	void action ();

	uint16_t currentTime ();

	void setDawnTime (uint16_t time);
	uint16_t getDawnTime ();

private:
	static void dawnStart ();
	uint16_t dawnTime; // in minutes
	bool dawnTriggered;
};

extern Alarms alarms;

#endif // ifndef ALARMS_H