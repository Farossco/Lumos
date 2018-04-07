#ifndef LIGHT_H
#define LIGHT_H

#include <Adafruit_DotStar.h>
#include <Arduino.h>

#define CURRENT_MOD 255

const uint8_t STRIP_LENGTH = 72;
const uint8_t PIN_DATA     = 2;
const uint8_t PIN_CLOCK    = 3;
const uint8_t PIN_MUSIC_IN = 0;

const boolean LIGHT_START_ANIMATION_ENABLE = true;

// Mods
const uint8_t LIGHT_MOD_MIN        = 0; // -Minimum mod value-
const uint8_t LIGHT_MOD_CONTINUOUS = 0; // Continuous lightning mod
const uint8_t LIGHT_MOD_FLASH      = 1; // Flash mod
const uint8_t LIGHT_MOD_STROBE     = 2; // Strobe mod
const uint8_t LIGHT_MOD_FADE       = 3; // Fade mod
const uint8_t LIGHT_MOD_SMOOTH     = 4; // Smooth mod
const uint8_t LIGHT_MOD_DAWN       = 5; // Dawn mod
const uint8_t LIGHT_MOD_SUNSET     = 6; // Sunset mod
const uint8_t LIGHT_MOD_START_ANIM = 7; // Start animation mod
const uint8_t LIGHT_MOD_MUSIC      = 8; // Music mod
const uint8_t LIGHT_MOD_MAX        = 8; // -Maximum mod value-

const uint8_t LIGHT_N_MOD = LIGHT_MOD_MAX - LIGHT_MOD_MIN + 1; // --Number of different mods--

//                                                  CONT  FLAS  STRO  FADE  SMOO  DAWN  SUNS  STAN  MUSI
const unsigned char DEFAULT_RED[LIGHT_N_MOD]    = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00 }; // Default red color on program startup
const unsigned char DEFAULT_GREEN[LIGHT_N_MOD]  = { 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x00, 0x00 }; // Default green color on program startup
const unsigned char DEFAULT_BLUE[LIGHT_N_MOD]   = { 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00 }; // Default blue color on program startup
const unsigned int DEFAULT_SPEED[LIGHT_N_MOD]   = { 0, 10, 10, 300, 500, 0, 0, 1000, 0000 };                // Default speed on program startup
const unsigned int LIGHT_MIN_SPEED[LIGHT_N_MOD] = { 0, 1, 1, 50, 10, 0, 0, 1, 000 };                        // Minimum speed or power value for each mod
const unsigned int LIGHT_MAX_SPEED[LIGHT_N_MOD] = { 0, 25, 25, 600, 1000, 0, 0, 10000, 000 };               // Maximum speed or power value for each mod
const unsigned char LIGHT_MIN_POWER             = 0;                                                        // Minimum power value
const unsigned char LIGHT_MAX_POWER             = 25;                                                       // Maximum power value

const unsigned char DEFAULT_POWER[LIGHT_N_MOD] = { LIGHT_MAX_POWER, LIGHT_MAX_POWER, LIGHT_MAX_POWER, LIGHT_MAX_POWER, LIGHT_MAX_POWER, LIGHT_MAX_POWER, LIGHT_MAX_POWER, LIGHT_MAX_POWER }; // Default power on program startup

class Light
{
public:
	Light();
	void init ();
	void reset ();
	void action (); // Perform mod actions
	void lightAll (unsigned char red, unsigned char green, unsigned char blue);
	void lightAll (unsigned long rgb);

	void setRed (unsigned char newRed, unsigned char affectedMod);
	void setGreen (unsigned char newGreen, unsigned char affectedMod);
	void setBlue (unsigned char newBlue, unsigned char affectedMod);
	void setRgb (unsigned long newRgb, unsigned char affectedMod);
	void setPower (unsigned char newPower, unsigned char affectedMod);
	void setSpeed (int newSpeed, unsigned char affectedMod);
	void setMod (unsigned char newMod);
	void switchOn ();
	void switchOff ();

	unsigned char getRed (unsigned char affectedMod);
	unsigned char getGreen (unsigned char affectedMod);
	unsigned char getBlue (unsigned char affectedMod);
	unsigned long getRgb (unsigned char affectedMod);
	unsigned char getPower (unsigned char affectedMod);
	unsigned int getSpeed (unsigned char affectedMod);
	unsigned char getMod ();
	bool isOn ();
	bool isOff ();

	int getDawnDuration ();

private:
	void initContinuous (); // Default mod initialization
	void continuous ();     // Default mod

	void initFlash (); // Flash mod initialization
	void flash ();     // Flash mod

	void initStrobe (); // Strobe mod initialization
	void strobe ();     // Strobe mod

	void initFade (); // Fade mod initialization
	void fade ();     // Fade mod

	void initSmooth (); // Smooth mod Initialization
	void smooth ();     // Smooth mod

	void initDawn (); // Dawn mod initialization
	void dawn ();     // Dawn mod

	void initSunset (); // Sunset mod initialization
	void sunset ();     // Sunset mod

	void initStartAnimation (); // Start animation mod initialization
	void startAnimation ();     // Start animation mod

	void initMusic (); // Music mod initialization
	void music ();     // Music mod

	Adafruit_DotStar strip;

	int state;                             // Current state used by some mods
	int32_t counter, counter2;             // Counters that are used by some mods
	unsigned long delayCount, delayCount2; // Delay counters
	unsigned char lastMod;                 // Mod in previous loop - Allows mod initializations
	bool on;                               // If the leds are ON or OFF (True: ON / False: OFF)
	unsigned char power[LIGHT_N_MOD];      // Current lightning power for each mod (from MINPOWER to MAXPOWER)
	unsigned int speed[LIGHT_N_MOD];       // Current mod speed for each mod
	unsigned char red[LIGHT_N_MOD];        // Current red value for each mod including lightning power (From 0 to 255)
	unsigned char green[LIGHT_N_MOD];      // Current green value for each mod including lightning power (From 0 to 255)
	unsigned char blue[LIGHT_N_MOD];       // Current blue value for each mod including lightning power (From 0 to 255)
	unsigned char mod;                     // Current lighting mod (MOD_***)

	// Start animation variables
	const int step = 3500 / STRIP_LENGTH;
	int tempRed;
	int tempGreen;
	int tempBlue;
};

extern Light light;

#endif // ifndef LIGHT_H
