#include "Sound.h"
#include "Logger.h"

Sound::Sound() : myDFPlayer()
{ }

void Sound::init (HardwareSerial &serial)
{
	if (!SOUND_ENABLED)
		return;

	Log.info ("Initializing sound... ");

	serial.begin (DFP_BAUD_RATE);

	if (myDFPlayer.begin (serial))
	{
		Log.infonp ("Done." dendl);
	}
	else
	{
		Log.infonp ("Failed!" dendl);
		Log.error ("DFPlayer communication failed! No sound for this session..." dendl);
		return;
	}

	volume = SOUND_VOLUME_DEFAULT;

	myDFPlayer.pause();
}

void Sound::action ()
{
	// If lightning is off, shut all lights
	if (isOff())
	{
		lastMod = -1;
		return;
	}

	switch (lastMod)
	{
		case SOUND_MOD_FREE:
			if (mod != SOUND_MOD_FREE)
			{
				Log.info ("Leaving Free choice mod" dendl);
			}
			break;
	}

	// Calling mods functions
	switch (mod)
	{
		case SOUND_MOD_FREE:
			if (lastMod != SOUND_MOD_FREE) // If this is first call of the function, we call init function (lastMod will be set in init function)
			{
				lastMod = SOUND_MOD_FREE;
				Log.info ("Entering Free choice mod" dendl);
			}

			break;
	}
} // Sound::action

void Sound::command (uint8_t command, int32_t information)
{
	switch (command)
	{
		case SOUND_COMMAND_PLAY:
			myDFPlayer.volume (volume);
			myDFPlayer.playMp3Folder (information);
			Log.trace ("Playing music nb %d with volume %d/%d" dendl, information, volume, SOUND_VOLUME_MAX);
			break;

	}
}

void Sound::setVolume (uint8_t newVolume)
{
	volume = newVolume;
	myDFPlayer.volume (volume);
}

void Sound::setMod (uint8_t newMod)
{
	mod = newMod;
}

void Sound::switchOn ()
{
	on = true;
}

void Sound::switchOff ()
{
	on = false;
}

uint8_t Sound::getVolume ()
{
	return volume;
}

uint8_t Sound::getMod ()
{
	return mod;
}

bool Sound::isOn ()
{
	return on != 0;
}

bool Sound::isOff ()
{
	return on == 0;
}

Sound sound = Sound();
