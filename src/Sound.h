#ifndef SOUND_H
#define SOUND_H

#include <DFRobotDFPlayerMini.h>
#include <Arduino.h>

const long DFP_BAUD_RATE = 9600; // DFPlayer communication baud rate

const boolean SOUND_ENABLED        = true; // Enable sound
const uint8_t SOUND_VOLUME_DEFAULT = 5;    // DFPlayer default volume (0 - 30)
const uint8_t SOUND_VOLUME_MIN     = 0;    // Minimum DFPlayer volume
const uint8_t SOUND_VOLUME_MAX     = 30;   // Maximum DFPlayer volume

// Mods
const uint8_t SOUND_MOD_MIN  = 0; // -Minimum mod value-
const uint8_t SOUND_MOD_FREE = 0; // Free choice mod
const uint8_t SOUND_MOD_MAX  = 0; // -Maximum mod value-

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
	void setMod (uint8_t newMod);
	void switchOn ();
	void switchOff ();

	uint8_t getVolume ();
	uint8_t getMod ();
	boolean isOn ();
	boolean isOff ();

private:
	DFRobotDFPlayerMini myDFPlayer;
	uint8_t volume;
	uint8_t mod;
	uint8_t lastMod;
	bool on;
};

extern Sound sound;

#endif // ifndef SOUND_H
