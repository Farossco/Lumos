#include "Sound.h"
#include "Logger.h"

Sound::Sound()
{ }

void Sound::setVolume (uint8_t newVolume)
{
	volume = newVolume;
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
