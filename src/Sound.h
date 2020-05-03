#ifndef SOUND_H
#define SOUND_H

#include <Arduino.h>
#include "Types.h"

#if defined(LUMOS_ARDUINO_MEGA)
# include <DFRobotDFPlayerMini.h>
#endif

const uint32_t DFP_BAUD_RATE = 9600; // DFPlayer communication baud rate

const boolean SOUND_ENABLED = true; // Enable sound

class Sound
{
public:
	Sound();
	void init (HardwareSerial &serial);

	void action ();
	void command (SoundCommand command, uint32_t information);

	void setVolume (SoundVolume newVolume);
	void setMode (SoundMode newMode);
	void switchOn ();
	void switchOff ();

	SoundVolume getVolume ();
	SoundMode getMode ();
	bool isOn ();
	bool isOff ();

	SoundMode mode;
	SoundVolume volume;
	SoundOnOff on;

private:
	SoundMode lastMode;

	#if defined(LUMOS_ARDUINO_MEGA)
	DFRobotDFPlayerMini myDFPlayer;
	#endif
};

extern Sound sound;

#endif // ifndef SOUND_H