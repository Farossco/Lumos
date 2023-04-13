#define SOUND_H

#include <Arduino.h>
#include "Types.h"

#include <DFRobotDFPlayerMini.h>

enum {
	SOUND_COMMAND_PLAY_RANDOM,
	SOUND_COMMAND_PLAY_ONE,
	SOUND_COMMAND_PLAY_NEXT,
	SOUND_COMMAND_PLAY_PREVIOUS,
	SOUND_COMMAND_PAUSE,
	SOUND_COMMAND_RESUME,
	SOUND_COMMAND_PLAY_DAWN,

	SOUND_COMMAND_N,
	SOUND_COMMAND_MIN = 0,
	SOUND_COMMAND_MAX = SOUND_COMMAND_N - 1
};

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
