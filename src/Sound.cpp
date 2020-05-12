#include "Sound.h"
#include "ArduinoLogger.h"
#include "Memory.h"

#if defined(LUMOS_ARDUINO_MEGA)

Sound::Sound() : myDFPlayer()
{ }

#endif

#if defined(LUMOS_ESP8266)

Sound::Sound()
{ }

#endif

void Sound::setVolumeRaw (SoundVolume newVolume)
{
	volume = newVolume;

	#if defined(LUMOS_ARDUINO_MEGA)
	myDFPlayer.volume (volume.value());
	#endif
}

void Sound::setVolume (Percentage percent)
{
	volume = percent;

	#if defined(LUMOS_ARDUINO_MEGA)
	myDFPlayer.volume (volume.value());
	#endif
}

void Sound::setMode (SoundMode newMode)
{
	mode = newMode;
}

void Sound::switchOn ()
{
	on = SoundOnOff (true);
}

void Sound::switchOff ()
{
	on = SoundOnOff (false);
}

SoundVolume Sound::getVolume ()
{
	return volume;
}

SoundMode Sound::getMode ()
{
	return mode;
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

void Sound::init (HardwareSerial &soundSerial)
{
	if (!SOUND_ENABLED)
		return;

	if (memory.readSound())
	{
		inf << "This is first launch, sound variables will be initialized to their default values" << endl;

		volume = SoundVolume::DEF;
	}

	inf << "Initializing sound... ";

	soundSerial.begin (DFP_BAUD_RATE);

	if (myDFPlayer.begin (soundSerial))
	{
		inf << "Done." << dendl;
	}
	else
	{
		inf << "Failed!" << endl;
		err << "DFPlayer communication failed! No sound for this session..." << dendl;
	}

	myDFPlayer.pause();
	setVolumeRaw (volume);
}

void Sound::action ()
{
	if (isOff())
	{
		lastMode = -1;
		// TODO : actually stop the music
		return;
	}

	// Calling modes functions
	switch (mode)
	{
		case SoundMode::freeChoice:
			if (lastMode != mode) // If this is first call of the function, we call init function (lastMode will be set in init function)
			{
				lastMode = mode;
				inf << "Entering Free choice mode" << dendl;
			}

			break;
	}
}

void Sound::command (SoundCommand command, uint32_t information)
{
	if (mode != SoundMode::freeChoice)
		return;

	switch (command)
	{
		case SoundCommand::playRandom:
			setVolumeRaw (volume);
			myDFPlayer.randomAll();
			trace << "Playing random mp3 until stop" << dendl;
			break;

		case SoundCommand::playOne:
			setVolumeRaw (volume);
			myDFPlayer.playFolder (2, information);
			trace << "Playing mp3 " << information << " with volume " << volume << dendl;
			break;

		case SoundCommand::playNext:
			setVolumeRaw (volume);
			myDFPlayer.next();
			trace << "Playing next mp3" << dendl;
			break;

		case SoundCommand::playPrevious:
			setVolumeRaw (volume);
			myDFPlayer.previous();
			trace << "Playing previous mp3" << dendl;
			break;

		case SoundCommand::pause:
			setVolumeRaw (volume);
			myDFPlayer.pause();
			trace << "Pausing mp3 play" << dendl;
			break;

		case SoundCommand::resume:
			setVolumeRaw (volume);
			myDFPlayer.start();
			trace << "Resuming mp3 play" << dendl;
			break;

		case SoundCommand::playDawn:
			setVolumeRaw (volume);
			myDFPlayer.playFolder (1, 1);
			trace << "Playing dawn mp3" << dendl;
			break;
	}
} // Sound::command

#endif // if defined(LUMOS_ARDUINO_MEGA)

Sound sound = Sound();