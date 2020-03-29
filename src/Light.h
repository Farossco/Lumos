#ifndef LIGHT_H
#define LIGHT_H

#if defined(LUMOS_ARDUINO_MEGA)
# include <Adafruit_DotStar.h>
#endif

#include <Arduino.h>

#define CURRENT_MOD 255

const uint8_t STRIP_LENGTH = 72;
const uint8_t PIN_DATA     = 50;
const uint8_t PIN_CLOCK    = 52;
const uint8_t PIN_MUSIC_IN = 0;

const boolean LIGHT_START_ANIMATION_ENABLE = true;

// Modes
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

const uint8_t LIGHT_MOD_N = LIGHT_MOD_MAX - LIGHT_MOD_MIN + 1; // --Number of different mods--

//                                              CONT  FLAS  STRO  FADE  SMOO  DAWN  SUNS  STAN  MUSI
const uint8_t DEFAULT_RED[LIGHT_MOD_N]      = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00 }; // Default red color on program startup
const uint8_t DEFAULT_GREEN[LIGHT_MOD_N]    = { 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x00, 0x00 }; // Default green color on program startup
const uint8_t DEFAULT_BLUE[LIGHT_MOD_N]     = { 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00 }; // Default blue color on program startup
const uint16_t DEFAULT_SPEED[LIGHT_MOD_N]   = { 0, 10, 10, 300, 500, 0, 0, 1000, 0000 };                // Default speed on program startup
const uint16_t LIGHT_MIN_SPEED[LIGHT_MOD_N] = { 0, 1, 1, 50, 10, 0, 0, 1, 000 };                        // Minimum speed or power value for each mod
const uint16_t LIGHT_MAX_SPEED[LIGHT_MOD_N] = { 0, 25, 25, 600, 1000, 0, 0, 10000, 000 };               // Maximum speed or power value for each mod
const uint8_t LIGHT_MIN_POWER               = 5;                                                        // Minimum power value
const uint8_t LIGHT_MAX_POWER               = 25;                                                       // Maximum power value
const uint8_t LIGHT_MIN_COLOR               = 5;                                                        // Maximum color value
const uint8_t LIGHT_MAX_COLOR               = 255;                                                      // Maximum color value
const uint32_t LIGHT_MIN_RGB                = 0x00000000;                                               // Maximum RGB value
const uint32_t LIGHT_MAX_RGB                = 0x00FFFFFF;                                               // Maximum RGB value

const uint8_t DEFAULT_POWER[LIGHT_MOD_N] = { LIGHT_MAX_POWER, LIGHT_MAX_POWER, LIGHT_MAX_POWER, LIGHT_MAX_POWER, LIGHT_MAX_POWER, LIGHT_MAX_POWER, LIGHT_MAX_POWER, LIGHT_MAX_POWER }; // Default power on program startup

class Light
{
public:
	Light();
	void init ();
	void reset ();
	void action (); // Perform mod actions
	void lightAll (uint8_t red, uint8_t green, uint8_t blue);
	void lightAll (uint32_t rgb);

	void setRed (uint8_t newRed, uint8_t affectedMod      = CURRENT_MOD);
	void setGreen (uint8_t newGreen, uint8_t affectedMod  = CURRENT_MOD);
	void setBlue (uint8_t newBlue, uint8_t affectedMod    = CURRENT_MOD);
	void setRgb (uint32_t newRgb, uint8_t affectedMod     = CURRENT_MOD);
	void setPower (uint8_t newPower, uint8_t affectedMod  = CURRENT_MOD);
	void setSpeed (uint16_t newSpeed, uint8_t affectedMod = CURRENT_MOD);
	void setMod (uint8_t newMod);
	void switchOn ();
	void switchOff ();

	uint8_t addPower (uint8_t newPower, uint8_t affectedMod      = CURRENT_MOD);
	uint8_t subtractPower (uint8_t powerAdd, uint8_t affectedMod = CURRENT_MOD);

	uint8_t getRed (uint8_t affectedMod    = CURRENT_MOD);
	uint8_t getGreen (uint8_t affectedMod  = CURRENT_MOD);
	uint8_t getBlue (uint8_t affectedMod   = CURRENT_MOD);
	uint32_t getRgb (uint8_t affectedMod   = CURRENT_MOD);
	uint8_t getPower (uint8_t affectedMod  = CURRENT_MOD);
	uint16_t getSpeed (uint8_t affectedMod = CURRENT_MOD);
	uint8_t getMod ();
	bool isOn ();
	bool isOff ();

	uint16_t getDawnDuration ();

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

	uint8_t state;                    // Current state used by some mods
	int32_t counter, counter2;        // Counters that are used by some mods
	uint32_t delayCount, delayCount2; // Delay counters
	uint8_t lastMod;                  // Mod in previous loop - Allows mod initializations
	bool on;                          // If the leds are ON or OFF (True: ON / False: OFF)
	uint8_t power[LIGHT_MOD_N];       // Current lightning power for each mod (from MINPOWER to MAXPOWER)
	uint16_t speed[LIGHT_MOD_N];      // Current mod speed for each mod
	uint8_t red[LIGHT_MOD_N];         // Current red value for each mod including lightning power (From 0 to 255)
	uint8_t green[LIGHT_MOD_N];       // Current green value for each mod including lightning power (From 0 to 255)
	uint8_t blue[LIGHT_MOD_N];        // Current blue value for each mod including lightning power (From 0 to 255)
	uint8_t mod;                      // Current lighting mod (MOD_***)

	// Start animation variables
	const uint16_t step = 6 * 255 / (STRIP_LENGTH / 4);
	uint8_t tempRed;
	uint8_t tempGreen;
	uint8_t tempBlue;

	#if defined(LUMOS_ARDUINO_MEGA)
	Adafruit_DotStar strip;
	#endif
};

extern Light light;

#endif // ifndef LIGHT_H