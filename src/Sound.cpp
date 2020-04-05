#include "Sound.h"
#include "Logger.h"

#if defined(LUMOS_ARDUINO_MEGA)

Sound::Sound() : myDFPlayer()
{ }

#endif

#if defined(LUMOS_ESP8266)

Sound::Sound()
{ }

#endif

void Sound::setVolume (uint8_t newVolume)
{
	volume = newVolume;

	#if defined(LUMOS_ARDUINO_MEGA)
	myDFPlayer.volume (volume);
	#endif
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

#if defined(LUMOS_ARDUINO_MEGA)

void Sound::init (HardwareSerial &serial)
{
	if (!SOUND_ENABLED)
		return;

	inf << "Initializing sound... ";

	serial.begin (DFP_BAUD_RATE);

	if (myDFPlayer.begin (serial))
	{
		inf << "Done." << dendl;
	}
	else
	{
		inf << "Failed!" << endl;
		err << "DFPlayer communication failed! No sound for this session..." << dendl;
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
				inf << "Leaving Free choice mod" << dendl;
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
				inf << "Entering Free choice mod" << dendl;
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
			trace << "Playing random mp3 until stop" << dendl;
			break;

		case SOUND_COMMAND_PLAY_ONE:
			myDFPlayer.volume (volume);
			myDFPlayer.playFolder (2, (uint8_t) information);
			trace << "Playing mp3 " << (uint16_t) information << " with volume " << volume << "/" << SOUND_VOLUME_MAX << dendl;
			break;

		case SOUND_COMMAND_PLAY_NEXT:
			myDFPlayer.volume (volume);
			myDFPlayer.next();
			trace << "Playing next mp3" << dendl;
			break;

		case SOUND_COMMAND_PLAY_PREVIOUS:
			myDFPlayer.volume (volume);
			myDFPlayer.previous();
			trace << "Playing previous mp3" << dendl;
			break;

		case SOUND_COMMAND_PAUSE:
			myDFPlayer.volume (volume);
			myDFPlayer.pause();
			trace << "Pausing mp3 play" << dendl;
			break;

		case SOUND_COMMAND_RESUME:
			myDFPlayer.volume (volume);
			myDFPlayer.start();
			trace << "Resuming mp3 play" << dendl;
			break;

		case SOUND_COMMAND_PLAY_DAWN:
			myDFPlayer.volume (volume);
			myDFPlayer.playFolder (1, 1);
			trace << "Playing dawn mp3" << dendl;
			break;
	}
} // Sound::command

#endif // if defined(LUMOS_ARDUINO_MEGA)

Sound sound = Sound();