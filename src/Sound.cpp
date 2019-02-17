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
	myDFPlayer.volume (volume);
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
	if (mod != SOUND_MOD_FREE)
		return;

	switch (command)
	{
		case SOUND_COMMAND_PLAY_RANDOM:
			myDFPlayer.volume (volume);
			myDFPlayer.randomAll();
			Log.trace ("Playing random mp3 until stop" dendl);
			break;

		case SOUND_COMMAND_PLAY_ONE:
			myDFPlayer.volume (volume);
			myDFPlayer.playFolder (2, (uint8_t) information);
			Log.trace ("Playing mp3 %d with volume %d/%d" dendl, (uint16_t) information, volume, SOUND_VOLUME_MAX);
			break;

		case SOUND_COMMAND_PLAY_NEXT:
			myDFPlayer.volume (volume);
			myDFPlayer.next();
			Log.trace ("Playing next mp3" dendl);
			break;

		case SOUND_COMMAND_PLAY_PREVIOUS:
			myDFPlayer.volume (volume);
			myDFPlayer.previous();
			Log.trace ("Playing previous mp3" dendl);
			break;

		case SOUND_COMMAND_PAUSE:
			myDFPlayer.volume (volume);
			myDFPlayer.pause();
			Log.trace ("Pausing mp3 play" dendl);
			break;

		case SOUND_COMMAND_RESUME:
			myDFPlayer.volume (volume);
			myDFPlayer.start();
			Log.trace ("Resuming mp3 play" dendl);
			break;

		case SOUND_COMMAND_PLAY_DAWN:
			myDFPlayer.volume (volume);
			myDFPlayer.playFolder (1, 1);
			Log.trace ("Playing dawn mp3" dendl);
			break;
	}
} // Sound::command

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
