#include "Alarms.h"
#include "ArduinoLogger.h"
#include "Light.h"
#include "Memory.h"
#include "Sound.h"

Alarms::Alarms()
{ }

void Alarms::setDawnVolumeRaw (SoundVolume volume){ dawnVolume = volume; }

void Alarms::setDawnVolume (Percentage percent){ dawnVolume = percent; }

SoundVolume Alarms::getDawnVolume (){ return dawnVolume; }

void Alarms::setDawnTime (Time time){ dawnTime = time; }

Time Alarms::getDawnTime (){ return dawnTime; }

void Alarms::setSunsetDuration (Timing timing){ sunsetDuration = timing; }

Timing Alarms::getSunsetDuration (){ return sunsetDuration; }

void Alarms::setDawnDuration (Timing timing){ dawnDuration = timing; }

Timing Alarms::getDawnDuration (){ return dawnDuration; }

void Alarms::setSunsetDecreaseTime (Timing timing){ sunsetDecreaseTime = timing; }

Timing Alarms::getSunsetDecreaseTime (){ return sunsetDecreaseTime; }

#if defined(LUMOS_ARDUINO_MEGA)

Time Alarms::currentTime (){ return Time (hour(), minute()); }

void Alarms::init ()
{
	dawnTriggered = false;

	if (memory.readAlarms()) // Returns True if EEPROM is not correctly initialized (This may be the first launch)
	{
		inf << "This is first launch, alarm variables will be initialized to their default values" << endl;

		dawnTime           = DEFAULT_DAWN_TIME;
		dawnDuration       = DEFAULT_DAWN_DURATION;
		sunsetDuration     = DEFAULT_SUNSET_DURATION;
		sunsetDecreaseTime = DEFAULT_SUNSET_DECREASE_TIME;
		dawnVolume         = SoundVolume::DEF;
	}

	inf << "Alarms initialized." << dendl;
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

void Alarms::dawnStart ()
{
	light.setMode (LightMode::dawn);
	light.switchOn();
	sound.setVolumeRaw (dawnVolume);
	sound.setMode (SoundMode::freeChoice);
	sound.command (SoundCommand::playDawn, 0);

	inf << "Entering dawn mode from Alarms" << dendl;
}

#endif // if defined(LUMOS_ARDUINO_MEGA)

Alarms alarms = Alarms();