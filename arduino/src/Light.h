#ifndef LIGHT_H
#define LIGHT_H

#include <Arduino.h>
#include <Adafruit_DotStar.h>

#define CURRENT_MOD 255

const int STRIP_LENGTH = 72;
const int PIN_DATA     = 2;
const int PIN_CLOCK    = 3;

const boolean START_ANIMATION_ENABLE = true;
const int START_ANIMATION_DURATION   = 5000;

// Mods
const int MOD_MIN        = 0; // -Minimum mod value-
const int MOD_CONTINUOUS = 0; // Continuous lightning mod
const int MOD_FLASH      = 1; // Flash mod
const int MOD_STROBE     = 2; // Strobe mod
const int MOD_FADE       = 3; // Fade mod
const int MOD_SMOOTH     = 4; // Smooth mod
const int MOD_DAWN       = 5; // Dawn mod
const int MOD_START_ANIM = 6; // Start animation mod
const int MOD_MAX        = 6; // -Maximum mod value-

const int N_MOD = MOD_MAX - MOD_MIN + 1; // --Number of different mods--

//                                           CONT  FLAS  STRO  FADE  SMOO  DAWN  STAN
const unsigned char DEFAULT_RED[N_MOD]   = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00 };  // Default red color on program startup
const unsigned char DEFAULT_GREEN[N_MOD] = { 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00 };  // Default green color on program startup
const unsigned char DEFAULT_BLUE[N_MOD]  = { 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00 };  // Default blue color on program startup
const unsigned char DEFAULT_POWER[N_MOD] = { 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F };  // Default power on program startup
const int DEFAULT_SPEED[N_MOD]           = { 0000, 0010, 0010, 0300, 0500, 0500, 5000 };  // Default speed on program startup
const int MIN_SPEED[N_MOD]               = { 0000, 0001, 0001, 0050, 0010, 0001, 00000 }; // Minimum speed or power value for each mod
const int MAX_SPEED[N_MOD]               = { 0000, 0025, 0025, 0600, 1000, 1000, 10000 }; // Maximum speed or power value for each mod
const unsigned char MIN_POWER            = 0;                                             // Minimum power value
const unsigned char MAX_POWER            = 255;                                           // Maximum power value

class Light
{
public:
	Light();
	void init ();
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
	int getSpeed (unsigned char affectedMod);
	unsigned char getMod ();
	boolean isOn ();
	boolean isOff ();

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

	void initStartAnimation ();
	void startAnimation ();

	Adafruit_DotStar strip;

	int state;                  // Current state used by some mods
	int counter;                // Counter that is used by some mods
	unsigned long delayCount;   // Delay counting
	unsigned char lastMod;      // Mod in previous loop - Allows mod initializations
	boolean on;                 // If the leds are ON or OFF (True: ON / False: OFF)
	unsigned char power[N_MOD]; // Current lightning power for each mod (from MINPOWER to MAXPOWER)
	int speed[N_MOD];           // Current mod speed for each mod
	unsigned char red[N_MOD];   // Current red value for each mod including lightning power (From 0 to 255)
	unsigned char green[N_MOD]; // Current green value for each mod including lightning power (From 0 to 255)
	unsigned char blue[N_MOD];  // Current blue value for each mod including lightning power (From 0 to 255)
	unsigned char mod;          // Current lighting mod (MOD_***)

	// Start animation variables
	const int step = 5000 / STRIP_LENGTH + 1;
	int tempRed;
	int tempGreen;
	int tempBlue;
};

extern Light light;

#endif // ifndef LIGHT_H
