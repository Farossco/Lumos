#include "Alarms.h"
#include "Logger.h"
#include "Light.h"
#include "Memory.h"

Alarms::Alarms()
{ }

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

void Alarms::setDawnTime (uint16_t time)
{
	dawnTime = time;
}

uint16_t Alarms::getDawnTime ()
{
	return dawnTime;
}

void Alarms::dawnStart ()
{
	light.setMod (LIGHT_MOD_DAWN);
	light.switchOn();

	Log.info ("Entering dawn mod from Alarms" dendl);
}

Alarms alarms = Alarms();
