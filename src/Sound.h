#ifndef SOUND_H
#define SOUND_H

#include <Arduino.h>
#include "Types.h"

#include <DFRobotDFPlayerMini.h>

const uint32_t DFP_BAUD_RATE = 9600; /* DFPlayer communication baud rate */

const boolean SOUND_ENABLED = true; /* Enable sound */

class Sound {
public:
	Sound();
	void init(HardwareSerial &serial);

	void action();
	void command(SoundCommand command, uint32_t information);

	void setVolumeRaw(SoundVolume newVolume);
	void setVolume(Percentage percent);
	void setMode(SoundMode newMode);
	void switchOn();
	void switchOff();

	SoundVolume getVolume();
	SoundMode getMode();
	bool isOn();
	bool isOff();

	SoundMode mode;
	SoundVolume volume;
	SoundOnOff on;

private:
	SoundMode lastMode;

	DFRobotDFPlayerMini myDFPlayer;
};

extern Sound sound;

#endif // ifndef SOUND_H
