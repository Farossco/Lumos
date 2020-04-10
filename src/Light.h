#ifndef LIGHT_H
#define LIGHT_H

#if defined(LUMOS_ARDUINO_MEGA)
# include <Adafruit_DotStar.h>
#endif

#include <Arduino.h>
#include "Utils.h"

#define CURRENT_MODE 255
#define FULL_LINE    0x000000

const uint8_t STRIP_LENGTH = 72;
const uint8_t PIN_DATA     = 50;
const uint8_t PIN_CLOCK    = 52;
const uint8_t PIN_MUSIC_IN = 0;

const boolean LIGHT_START_ANIMATION_ENABLE = true;

// Modes
const uint8_t LIGHT_MOD_MIN        = 0; // -Minimum mode value-
const uint8_t LIGHT_MOD_CONTINUOUS = 0; // Continuous lightning mode
const uint8_t LIGHT_MOD_FLASH      = 1; // Flash mode
const uint8_t LIGHT_MOD_STROBE     = 2; // Strobe mode
const uint8_t LIGHT_MOD_FADE       = 3; // Fade mode
const uint8_t LIGHT_MOD_SMOOTH     = 4; // Smooth mode
const uint8_t LIGHT_MOD_DAWN       = 5; // Dawn mode
const uint8_t LIGHT_MOD_SUNSET     = 6; // Sunset mode
const uint8_t LIGHT_MOD_START_ANIM = 7; // Start animation mode
const uint8_t LIGHT_MOD_MUSIC      = 8; // Music mode
const uint8_t LIGHT_MOD_MAX        = 8; // -Maximum mode value-

const uint8_t LIGHT_MOD_N = LIGHT_MOD_MAX - LIGHT_MOD_MIN + 1; // --Number of different modes--

//                                              CONTINUOUS      FLASH       STROBE        FADE        SMOOTH        DAWN        SUNSET       START         MUSIC
const uint8_t DEFAULT_POWER[LIGHT_MOD_N]    = { SEEKBAR_MAX, SEEKBAR_MAX, SEEKBAR_MAX, SEEKBAR_MAX, SEEKBAR_MAX, SEEKBAR_MAX, SEEKBAR_MAX, SEEKBAR_MAX, SEEKBAR_MAX }; // Default power on program startup
const uint8_t DEFAULT_RED[LIGHT_MOD_N]      = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00 };                                                                // Default red color on program startup
const uint8_t DEFAULT_GREEN[LIGHT_MOD_N]    = { 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x00, 0x00 };                                                                // Default green color on program startup
const uint8_t DEFAULT_BLUE[LIGHT_MOD_N]     = { 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00 };                                                                // Default blue color on program startup
const uint16_t DEFAULT_SPEED[LIGHT_MOD_N]   = { 0, 10, 10, 300, 500, 0, 0, 1000, 0000 };                                                                               // Default speed on program startup
const uint16_t LIGHT_MIN_SPEED[LIGHT_MOD_N] = { 0, 1, 1, 50, 10, 0, 0, 1, 000 };                                                                                       // Minimum speed or power value for each mode
const uint16_t LIGHT_MAX_SPEED[LIGHT_MOD_N] = { 0, 25, 25, 600, 1000, 0, 0, 10000, 000 };                                                                              // Maximum speed or power value for each mode
const uint8_t LIGHT_MIN_POWER               = 5;                                                                                                                       // Minimum power value
const uint8_t LIGHT_MAX_POWER               = 25;                                                                                                                      // Maximum power value
const uint8_t LIGHT_MIN_COLOR               = 0;                                                                                                                       // Maximum color value
const uint8_t LIGHT_MAX_COLOR               = 255;                                                                                                                     // Maximum color value
const uint32_t LIGHT_MIN_RGB                = 0x00000000;                                                                                                              // Maximum RGB value
const uint32_t LIGHT_MAX_RGB                = 0x00FFFFFF;                                                                                                              // Maximum RGB value

const uint32_t colorList[][6] =
{
	{ FULL_LINE, 0xFFFFFF                                         },
	{ 0xFF0000,  0xFF5500, 0xFFAA00, 0xFFFF00, 0xAAFF00, 0x55FF00 },
	{ 0x00FF00,  0x00FF55, 0x00FFAA, 0x00FFFF, 0x00AAFF, 0x0055FF },
	{ 0x0000FF,  0x5500FF, 0xAA00FF, 0xFF00FF, 0xFF00AA, 0xFF0055 }
};

class Light
{
public:
	Light();
	void init ();
	void reset ();
	void action (); // Perform mode actions
	void lightAll (uint8_t red, uint8_t green, uint8_t blue);
	void lightAll (uint32_t rgb);

	void setRed (uint8_t newRed, uint8_t affectedMod            = CURRENT_MODE);
	void setGreen (uint8_t newGreen, uint8_t affectedMod        = CURRENT_MODE);
	void setBlue (uint8_t newBlue, uint8_t affectedMod          = CURRENT_MODE);
	void setRgb (uint32_t newRgb, uint8_t affectedMod           = CURRENT_MODE);
	void setPowerRaw (uint8_t newPower, uint8_t affectedMod     = CURRENT_MODE);
	void setPowerPercent (uint8_t newPower, uint8_t affectedMod = CURRENT_MODE);
	void setSpeed (uint16_t newSpeed, uint8_t affectedMod       = CURRENT_MODE);
	void setMode (uint8_t newMode);
	void switchOn ();
	void switchOff ();

	uint8_t addPower (uint8_t newPower, uint8_t affectedMod      = CURRENT_MODE);
	uint8_t subtractPower (uint8_t powerAdd, uint8_t affectedMod = CURRENT_MODE);

	uint8_t getRed (uint8_t affectedMod          = CURRENT_MODE);
	uint8_t getGreen (uint8_t affectedMod        = CURRENT_MODE);
	uint8_t getBlue (uint8_t affectedMod         = CURRENT_MODE);
	uint32_t getRgb (uint8_t affectedMod         = CURRENT_MODE);
	uint8_t getPowerRaw (uint8_t affectedMod     = CURRENT_MODE);
	uint8_t getPowerPercent (uint8_t affectedMod = CURRENT_MODE);
	uint16_t getSpeed (uint8_t affectedMod       = CURRENT_MODE);
	uint8_t getMode ();
	bool isOn ();
	bool isOff ();

	uint16_t getDawnDuration ();

private:
	void initContinuous (); // Default mode initialization
	void continuous ();     // Default mode

	void initFlash (); // Flash mode initialization
	void flash ();     // Flash mode

	void initStrobe (); // Strobe mode initialization
	void strobe ();     // Strobe mode

	void initFade (); // Fade mode initialization
	void fade ();     // Fade mode

	void initSmooth (); // Smooth mode Initialization
	void smooth ();     // Smooth mode

	void initDawn (); // Dawn mode initialization
	void dawn ();     // Dawn mode

	void initSunset (); // Sunset mode initialization
	void sunset ();     // Sunset mode

	void initStartAnimation (); // Start animation mode initialization
	void startAnimation ();     // Start animation mode

	void initMusic (); // Music mode initialization
	void music ();     // Music mode

	uint8_t state;                    // Current state used by some modes
	int32_t counter, counter2;        // Counters that are used by some modes
	uint32_t delayCount, delayCount2; // Delay counters
	uint8_t lastMode;                 // Mode in previous loop - Allows mode initializations
	bool on;                          // If the leds are ON or OFF (True: ON / False: OFF)
	uint8_t power[LIGHT_MOD_N];       // Current lightning power for each mode (from MINPOWER to MAXPOWER)
	uint16_t speed[LIGHT_MOD_N];      // Current mode speed for each mode
	uint8_t red[LIGHT_MOD_N];         // Current red value for each mode including lightning power (From 0 to 255)
	uint8_t green[LIGHT_MOD_N];       // Current green value for each mode including lightning power (From 0 to 255)
	uint8_t blue[LIGHT_MOD_N];        // Current blue value for each mode including lightning power (From 0 to 255)
	uint8_t mode;                     // Current lighting mode (MOD_***)

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