#ifndef SOUND_H
#define SOUND_H

#include <Arduino.h>

#if defined(LUMOS_ARDUINO_MEGA)
# include <DFRobotDFPlayerMini.h>
#endif

const uint32_t DFP_BAUD_RATE = 9600; // DFPlayer communication baud rate

const boolean SOUND_ENABLED        = true; // Enable sound
const uint8_t SOUND_VOLUME_DEFAULT = 5;    // DFPlayer default volume (0 - 30)
const uint8_t SOUND_VOLUME_MIN     = 0;    // Minimum DFPlayer volume
const uint8_t SOUND_VOLUME_MAX     = 30;   // Maximum DFPlayer volume

// Mods
const uint8_t SOUND_MOD_MIN  = 0; // -Minimum mode value-
const uint8_t SOUND_MOD_FREE = 0; // Free choice mode
const uint8_t SOUND_MOD_MAX  = 0; // -Maximum mode value-

const uint8_t SOUND_COMMAND_MIN           = 0;
const uint8_t SOUND_COMMAND_PLAY_RANDOM   = 0;
const uint8_t SOUND_COMMAND_PLAY_ONE      = 1;
const uint8_t SOUND_COMMAND_PLAY_NEXT     = 2;
const uint8_t SOUND_COMMAND_PLAY_PREVIOUS = 3;
const uint8_t SOUND_COMMAND_PAUSE         = 4;
const uint8_t SOUND_COMMAND_RESUME        = 5;
const uint8_t SOUND_COMMAND_PLAY_DAWN     = 6;
const uint8_t SOUND_COMMAND_MAX           = 6;

class Sound
{
public:
	Sound();
	void init (HardwareSerial &serial);

	void action ();
	void command (uint8_t command, int32_t information);

	void setVolume (uint8_t newVolume);
	void setMode (uint8_t newMode);
	void switchOn ();
	void switchOff ();

	uint8_t getVolume ();
	uint8_t getMode ();
	bool isOn ();
	bool isOff ();

private:
	uint8_t volume;
	uint8_t mode;
	uint8_t lastMode;
	bool on;

	#if defined(LUMOS_ARDUINO_MEGA)
	DFRobotDFPlayerMini myDFPlayer;
	#endif
};

extern Sound sound;

#endif // ifndef SOUND_H