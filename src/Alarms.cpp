#include "Alarms.h"
#include "Logger.h"
#include "Light.h"
#include "Memory.h"
#include "Sound.h"

Alarms::Alarms()
{ }

void Alarms::setDawnTime (uint16_t time)
{
	dawnTime = time;
}

uint16_t Alarms::getDawnTime ()
{
	return dawnTime;
}

#if defined(LUMOS_ARDUINO_MEGA)

void Alarms::init ()
{
	dawnTriggered = false;

	if (memory.readForAlarms())
	{
		dawnTime = DEFAULT_DAWN_TIME;
	}
}

void Alarms::action ()
{
	if (!MORNING_ALARM_ENABLED || timeStatus() == timeNotSet)
		return;

	if (currentTime() == dawnTime)
	{
		if (dawnTriggered == false)
		{
			dawnStart();
			dawnTriggered = true;
		}
	}
	else
	{
		dawnTriggered = false;
	}
}

uint16_t Alarms::currentTime ()
{
	return (uint16_t) (hour() * 60 + minute());
}

void Alarms::dawnStart ()
{
	light.setMod (LIGHT_MOD_DAWN);
	light.switchOn();
	sound.setVolume (15);
	sound.command (SOUND_COMMAND_PLAY_DAWN, 0);

	Log.info ("Entering dawn mod from Alarms" dendl);
}

#endif // if defined(LUMOS_ARDUINO_MEGA)

Alarms alarms = Alarms();
