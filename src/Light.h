#ifndef LIGHT_H
#define LIGHT_H

#if defined(LUMOS_ARDUINO_MEGA)
# include <Adafruit_DotStar.h>
#endif

#include <Arduino.h>
#include "Utils.h"
#include "time.h"
#include "Types.h"

const uint8_t STRIP_LENGTH = 72;
const uint8_t PIN_DATA     = 51;
const uint8_t PIN_CLOCK    = 52;
const uint8_t PIN_MUSIC_IN = 0;
const uint8_t PIN_STRIP_CS = 11;

const boolean LIGHT_START_ANIMATION_ENABLE = true;

const uint16_t LIGHT_SA_COLOR_STEP = 6 * 255 / (STRIP_LENGTH / 4);

extern class Light light;
class Light
{
public:
	Light();

	void lightAll (LightColor red, LightColor green, LightColor blue);
	void lightAll (LightRgb rgb);

	void setRed (LightColor newRed, LightMode affectedMod     = light.mode);
	void setGreen (LightColor newGreen, LightMode affectedMod = light.mode);
	void setBlue (LightColor newBlue, LightMode affectedMod   = light.mode);
	void setRgb (LightRgb newRgb, LightMode affectedMod       = light.mode);
	void setPower (Percentage newPower, LightMode affectedMod = light.mode);
	void setSpeed (Percentage newSpeed, LightMode affectedMod = light.mode);

	void setMode (LightMode newMode);
	void switchOn ();
	void switchOff ();

	void addPower (Percentage powerAdd, LightMode affectedMod      = light.mode);
	void subtractPower (Percentage powerSub, LightMode affectedMod = light.mode);

	LightColor getRed (LightMode affectedMod          = light.mode);
	LightColor getGreen (LightMode affectedMod        = light.mode);
	LightColor getBlue (LightMode affectedMod         = light.mode);
	LightRgb getRgb (LightMode affectedMod            = light.mode);
	LightPower getPowerRaw (LightMode affectedMod     = light.mode);
	Percentage getPowerPercent (LightMode affectedMod = light.mode);
	LightSpeed getSpeedRaw (LightMode affectedMod     = light.mode);
	Percentage getSpeedPercent (LightMode affectedMod = light.mode);
	LightMode getMode ();

	bool isOn ();
	bool isOff ();

	void init ();
	void reset ();
	void action (); // Perform mode actions

	LightOnOff on;            // If the leds are ON or OFF (True: ON / False: OFF)
	LightPowerArray powers;   // Current lightning power for each mode
	LightSpeedArray speeds;   // Current mode speed for each mode
	LightRgbArray rgbs;       // Current RGB value for each mode
	LightTimingArray timings; // Current mode timing for each mode
	LightMode mode;           // Current lighting mode

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