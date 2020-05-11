#ifndef ALARMS_H
#define ALARMS_H

#include "Types.h"

const bool MORNING_ALARM_ENABLED = true; // Enable morning alarm

// Wake up
const Time DEFAULT_DAWN_TIME              = Time (8, 30);   // 08h30
const Timing DEFAULT_DAWN_DURATION        = Timing (30, 0); // 30 min
const Timing DEFAULT_SUNSET_DURATION      = Timing (30, 0); // 30 min
const Timing DEFAULT_SUNSET_DECREASE_TIME = Timing (5, 0);  // 5 min

class Alarms
{
public:
	Alarms();

	void init ();
	void action ();

	Time currentTime ();

	void setDawnVolumeRaw (SoundVolume volume);
	void setDawnVolume (Percentage volume);
	SoundVolume getDawnVolume ();
	void setDawnTime (Time time);
	Time getDawnTime ();
	void setDawnDuration (Timing timing);
	Timing getDawnDuration ();
	void setSunsetDuration (Timing time);
	Timing getSunsetDuration ();
	void setSunsetDecreaseTime (Timing timing);
	Timing getSunsetDecreaseTime ();

	SoundVolume dawnVolume;    // Dawn sounds volume
	Time dawnTime;             // Dawn time in minutes
	Timing dawnDuration;       // The amount of time it takes to end the mode (in secondes)
	Timing sunsetDuration;     // Duration before starting to decrease (in secondes)
	Timing sunsetDecreaseTime; // The amount of time it takes to end the mode (in secondes)

private:
	void dawnStart ();

	bool dawnTriggered;
};

extern Alarms alarms;

#endif // ifndef ALARMS_H