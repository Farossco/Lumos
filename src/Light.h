#ifndef LIGHT_H
#define LIGHT_H

#if defined(LUMOS_ARDUINO_MEGA)
# include <Adafruit_DotStar.h>
#endif

#include <Arduino.h>
#include "Utils.h"
#include "time.h"
#include "Types.h"

#define FULL_LINE 0x000000

const uint8_t STRIP_LENGTH = 72;
const uint8_t PIN_DATA     = 51;
const uint8_t PIN_CLOCK    = 52;
const uint8_t PIN_MUSIC_IN = 0;
const uint8_t PIN_STRIP_CS = 11;

const boolean LIGHT_START_ANIMATION_ENABLE = true;

// TODO : move this elsewhere
const uint32_t colorList[][6] =
{
	{ FULL_LINE, 0xFFFFFF                                         },
	{ 0xFF0000,  0xFF5500, 0xFFAA00, 0xFFFF00, 0xAAFF00, 0x55FF00 },
	{ 0x00FF00,  0x00FF55, 0x00FFAA, 0x00FFFF, 0x00AAFF, 0x0055FF },
	{ 0x0000FF,  0x5500FF, 0xAA00FF, 0xFF00FF, 0xFF00AA, 0xFF0055 }
};

const uint16_t LIGHT_SA_COLOR_STEP = 6 * 255 / (STRIP_LENGTH / 4);

class Light;

extern Light light;

class Light
{
public:
	Light();

	void lightAll (uint8_t red, uint8_t green, uint8_t blue);
	void lightAll (uint32_t rgb);

	void setRed (uint8_t newRed, LightMode affectedMod             = light.mode);
	void setGreen (uint8_t newGreen, LightMode affectedMod         = light.mode);
	void setBlue (uint8_t newBlue, LightMode affectedMod           = light.mode);
	void setRgb (uint32_t newRgb, LightMode affectedMod            = light.mode);
	void setPowerRaw (uint8_t newPower, LightMode affectedMod      = light.mode);
	void setPowerPercent (uint8_t newPower, LightMode affectedMod  = light.mode);
	void setSpeedRaw (uint16_t newSpeed, LightMode affectedMod     = light.mode);
	void setSpeedPercent (uint16_t newSpeed, LightMode affectedMod = light.mode);

	void setMode (LightMode newMode);
	void switchOn ();
	void switchOff ();

	uint8_t addPower (uint8_t powerAdd, LightMode affectedMod      = light.mode);
	uint8_t subtractPower (uint8_t powerSub, LightMode affectedMod = light.mode);

	uint8_t getRed (LightMode affectedMod          = light.mode);
	uint8_t getGreen (LightMode affectedMod        = light.mode);
	uint8_t getBlue (LightMode affectedMod         = light.mode);
	uint32_t getRgb (LightMode affectedMod         = light.mode);
	uint8_t getPowerRaw (LightMode affectedMod     = light.mode);
	uint8_t getPowerPercent (LightMode affectedMod = light.mode);
	uint8_t getSpeedRaw (LightMode affectedMod     = light.mode);
	uint8_t getSpeedPercent (LightMode affectedMod = light.mode);
	LightMode getMode ();

	bool isOn ();
	bool isOff ();

	void init ();
	void reset ();
	void action (); // Perform mode actions

	bool on;            // If the leds are ON or OFF (True: ON / False: OFF)
	LightSetting power; // Current lightning power for each mode (from MINPOWER to MAXPOWER)
	LightSetting speed; // Current mode speed for each mode
	LightSetting red;   // Current red value for each mode including lightning power (From 0 to 255)
	LightSetting green; // Current green value for each mode including lightning power (From 0 to 255)
	LightSetting blue;  // Current blue value for each mode including lightning power (From 0 to 255)
	LightMode mode;     // Current lighting mode (MOD_***)

private:
	void show ();

	void modeActions ();
	void continuous ();     // Default mode
	void flash ();          // Flash mode
	void strobe ();         // Strobe mode
	void fade ();           // Fade mode
	void smooth ();         // Smooth mode
	void dawn ();           // Dawn mode
	void sunset ();         // Sunset mode
	void startAnimation (); // Start animation mode
	void music ();          // Music mode

	#if defined(LUMOS_ARDUINO_MEGA)
	uint8_t state;                  // Current state used by some modes
	int32_t counter, counter2;      // Counters that are used by some modes
	time_t delayCount, delayCount2; // Delay counters
	LightMode lastMode;             // Mode in previous loop - Allows mode initializations

	uint8_t tempRed;
	uint8_t tempGreen;
	uint8_t tempBlue;

	Adafruit_DotStar strip;
	#endif // if defined(LUMOS_ARDUINO_MEGA)
};

#endif // ifndef LIGHT_H